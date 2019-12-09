#include <Arduino.h>
#include "FastLED.h"
#include "constants.h"
#include "patterns.h"

#ifdef __AVR__
#define HAS_EEPROM
#endif

#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif

const uint8_t FRAME_PERIOD = 1000 / FRAMES_PER_SECOND;

CRGB colors[NUM_LEDS];

const uint8_t NUM_STATES = 7;  // number of patterns to cycle through

// system timer, incremented by one every time through the main loop
unsigned int loopCount = 0;

unsigned int seed = 0;  // used to initialize random number generator

// enumerate the possible patterns in the order they will cycle
enum Pattern {
  WarmWhiteShimmer = 0,
  RandomColorWalk = 1,
  TraditionalColors = 2,
  ColorExplosion = 3,
  Gradient = 4,
  BrightTwinkle = 5,
  Collision = 6,
  AllOff = 255
};
unsigned char pattern = TraditionalColors;
unsigned int maxLoops;  // go to next state when loopCount >= maxLoops

void initializeRandomSeed() {
  // initialize the random number generator with a seed obtained by
  // summing the voltages on the disconnected analog inputs
  for (int i = 0; i < 8; i++)
  {
    seed += analogRead(i);
  }
  #ifdef HAS_EEPROM
    seed += EEPROM.read(0);  // get part of the seed from EEPROM
  #endif
  randomSeed(seed);

  #ifdef HAS_EEPROM
    // save a random number in EEPROM to be used for random seed
    // generation the next time the program runs
    EEPROM.write(0, random(256));
  #endif
}

// initialization stuff
void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(colors, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  initializeRandomSeed();

  pinMode(AUTOCYCLE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(NEXT_PATTERN_BUTTON_PIN, INPUT_PULLUP);

  delay(10);  // give pull-ups time raise the input voltage
}

// This function detects if the optional next pattern button is pressed
// (connecting the pin to ground) and advances to the next pattern
// in the cycle if so.  It also debounces the button.
void handleNextPatternButton() {
  if (digitalRead(NEXT_PATTERN_BUTTON_PIN) == 0) {
    // if optional button is pressed
    while (digitalRead(NEXT_PATTERN_BUTTON_PIN) == 0) {
      // wait for button to be released
      while (digitalRead(NEXT_PATTERN_BUTTON_PIN) == 0);
      delay(10);  // debounce the button
    }
    loopCount = 0;  // reset timer
    pattern = ((unsigned char)(pattern+1))%NUM_STATES;  // advance to next pattern
  }
}

void showPattern() {
  if (pattern == WarmWhiteShimmer || pattern == RandomColorWalk) {
    // for these two patterns, we want to make sure we get the same
    // random sequence six times in a row (this provides smoother
    // random fluctuations in brightness/color)
    if (loopCount % 6 == 0) {
      seed = random(30000);
    }
    randomSeed(seed);
  }

  // call the appropriate pattern routine based on state; these
  // routines just set the colors in the colors array
  switch (pattern) {
    case WarmWhiteShimmer:
      // warm white shimmer for 300 loopCounts, fading over last 70
      maxLoops = 300;
      warmWhiteShimmer(loopCount > maxLoops - 70, colors, NUM_LEDS);
      break;

    case RandomColorWalk:
      // start with alternating red and green colors that randomly walk
      // to other colors for 400 loopCounts, fading over last 80
      maxLoops = 400;
      randomColorWalk(
        loopCount == 0 ? 1 : 0,
        loopCount > maxLoops - 80,
        colors,
        NUM_LEDS
      );
      break;

    case TraditionalColors:
      // repeating pattern of red, green, orange, blue, magenta that
      // slowly moves for 400 loopCounts
      maxLoops = 400;
      traditionalColors(colors, NUM_LEDS, loopCount, 2);
      break;

    case ColorExplosion:
      // bursts of random color that radiate outwards from random points
      // for 630 loop counts; no burst generation for the last 70 counts
      // of every 200 count cycle or over the over final 100 counts
      // (this creates a repeating bloom/decay effect)
      maxLoops = 630;
      colorExplosion(
        (loopCount % 200 > 130) || (loopCount > maxLoops - 100),
        colors,
        NUM_LEDS
      );
      break;

    case Gradient:
      // red -> white -> green -> white -> red ... gradiant that scrolls
      // across the strips for 250 counts; this pattern is overlaid with
      // waves of dimness that also scroll (at twice the speed)
      maxLoops = 250;
      gradient(colors, NUM_LEDS, loopCount);
      delay(6);  // add an extra 6ms delay to slow things down
      break;

    case BrightTwinkle:
      // random LEDs light up brightly and fade away; it is a very similar
      // algorithm to colorExplosion (just no radiating outward from the
      // LEDs that light up); as time goes on, allow progressively more
      // colors, halting generation of new twinkles for last 100 counts.
      maxLoops = 1200;
      if (loopCount < 400) {
        brightTwinkle(0, 1, 0, colors, NUM_LEDS);  // only white for first 400 loopCounts
      }
      else if (loopCount < 650) {
        brightTwinkle(0, 2, 0, colors, NUM_LEDS);  // white and red for next 250 counts
      }
      else if (loopCount < 900) {
        brightTwinkle(1, 2, 0, colors, NUM_LEDS);  // red, and green for next 250 counts
      }
      else {
        // red, green, blue, cyan, magenta, yellow for the rest of the time
        brightTwinkle(1, 6, loopCount > maxLoops - 100, colors, NUM_LEDS);
      }
      break;

    case Collision:
      // colors grow towards each other from the two ends of the strips,
      // accelerating until they collide and the whole strip flashes
      // white and fades; this repeats until the function indicates it
      // is done by returning 1, at which point we stop keeping maxLoops
      // just ahead of loopCount
      if (!collision(colors, NUM_LEDS, loopCount)) {
        maxLoops = loopCount + 2;
      }
      break;
  }
}

// main loop
void loop() {
  handleNextPatternButton();

  if (loopCount == 0) {
    // whenever timer resets, clear the LED colors array (all off)
    for (int i = 0; i < NUM_LEDS; i++) {
      colors[i] = CRGB(0, 0, 0);
    }
  }

  showPattern();

  // update the LED strips with the colors in the colors array
  EVERY_N_MILLISECONDS(FRAME_PERIOD) {
    FastLED.show();
  }
  loopCount++;  // increment our loop counter/timer.

  if (loopCount >= maxLoops && !digitalRead(AUTOCYCLE_SWITCH_PIN)) {
    // if the time is up for the current pattern and the optional hold
    // switch is not grounding the AUTOCYCLE_SWITCH_PIN, clear the
    // loop counter and advance to the next pattern in the cycle
    loopCount = 0;  // reset timer
    pattern = ((unsigned char)(pattern+1))%NUM_STATES;  // advance to next pattern
  }
}
