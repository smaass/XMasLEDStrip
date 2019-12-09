#include <Arduino.h>
#include "FastLED.h"


void randomWalk(
  unsigned char *val, unsigned char maxVal, unsigned char changeAmount,
  unsigned char directions
) {
  unsigned char walk = random(directions);  // direction of random walk
  if (walk == 0) {
    // decrease val by changeAmount down to a min of 0
    if (*val >= changeAmount) {
      *val -= changeAmount;
    }
    else {
      *val = 0;
    }
  }
  else if (walk == 1) {
    // increase val by changeAmount up to a max of maxVal
    if (*val <= maxVal - changeAmount) {
      *val += changeAmount;
    }
    else {
      *val = maxVal;
    }
  }
}


void fade(unsigned char *val, unsigned char fadeTime) {
  if (*val != 0) {
    unsigned char subAmt = *val >> fadeTime;  // val * 2^-fadeTime
    if (subAmt < 1)
      subAmt = 1;  // make sure we always decrease by at least 1
    *val -= subAmt;  // decrease value of byte pointed to by val
  }
}


void warmWhiteShimmer(unsigned char dimOnly, CRGB colors[], int numLeds) {
  const unsigned char maxBrightness = 120;  // cap on LED brighness
  const unsigned char changeAmount = 2;   // size of random walk step

  for (int i = 0; i < numLeds; i += 2) {
    // randomly walk the brightness of every even LED
    randomWalk(&colors[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 2);

    // warm white: red = x, green = 0.8x, blue = 0.125x
    colors[i].green = colors[i].red*4/5;  // green = 80% of red
    colors[i].blue = colors[i].red >> 3;  // blue = red/8

    // every odd LED gets set to a quarter the brighness of the preceding even LED
    if (i + 1 < numLeds) {
      colors[i+1] = CRGB(colors[i].red >> 2, colors[i].green >> 2, colors[i].blue >> 2);
    }
  }
}


void randomColorWalk(
  unsigned char initializeColors,
  unsigned char dimOnly,
  CRGB colors[],
  int numLeds
) {
  const unsigned char maxBrightness = 180;  // cap on LED brightness
  const unsigned char changeAmount = 3;  // size of random walk step

  // pick a good starting point for our pattern so the entire strip
  // is lit well (if we pick wrong, the last four LEDs could be off)
  unsigned char start;
  switch (numLeds % 7) {
    case 0:
      start = 3;
      break;
    case 1:
      start = 0;
      break;
    case 2:
      start = 1;
      break;
    default:
      start = 2;
  }

  for (int i = start; i < numLeds; i+=7) {
    if (initializeColors == 0) {
      // randomly walk existing colors of every seventh LED
      // (neighboring LEDs to these will be dimmer versions of the same color)
      randomWalk(&colors[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 3);
      randomWalk(&colors[i].green, maxBrightness, changeAmount, dimOnly ? 1 : 3);
      randomWalk(&colors[i].blue, maxBrightness, changeAmount, dimOnly ? 1 : 3);
    }
    else if (initializeColors == 1) {
      // initialize LEDs to alternating red and green
      if (i % 2) {
        colors[i] = CRGB(maxBrightness, 0, 0);
      }
      else {
        colors[i] = CRGB(0, maxBrightness, 0);
      }
    }
    else {
      // initialize LEDs to a string of random colors
      colors[i] = CRGB(random(maxBrightness), random(maxBrightness), random(maxBrightness));
    }

    // set neighboring LEDs to be progressively dimmer versions of the color we just set
    if (i >= 1) {
      colors[i-1] = CRGB(colors[i].red >> 2, colors[i].green >> 2, colors[i].blue >> 2);
    }
    if (i >= 2) {
      colors[i-2] = CRGB(colors[i].red >> 3, colors[i].green >> 3, colors[i].blue >> 3);
    }
    if (i + 1 < numLeds) {
      colors[i+1] = colors[i-1];
    }
    if (i + 2 < numLeds) {
      colors[i+2] = colors[i-2];
    }
  }
}


void traditionalColors(
  CRGB colors[],
  int numLeds,
  unsigned int loopCount,
  unsigned int cycleDelay
) {
  // loop counts to leave strip initially dark
  const unsigned char initialDarkCycles = 10;
  // loop counts it takes to go from full off to fully bright
  const unsigned char brighteningCycles = 20;
  const unsigned char width = 4;

  // leave strip fully off for 20 cycles
  if (loopCount < initialDarkCycles) {
    return;
  }

  // if numLeds is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (20) and is not smaller
  // than the actual LED count.
  unsigned int extendedLEDCount = (((numLeds-1)/20)+1)*20;

  for (int i = 0; i < extendedLEDCount; i++) {
    unsigned char brightness = (loopCount - initialDarkCycles)%brighteningCycles + 1;
    unsigned char cycle = (loopCount - initialDarkCycles)/brighteningCycles;

    // transform i into a moving idx space that translates one step per
    // brightening cycle and wraps around
    unsigned int idx = (i + cycle)%extendedLEDCount;
    // if our transformed index exists
    if (idx < numLeds) {
      if (i % 4 == 0) {
        // if this is an LED that we are coloring, set the color based
        // on the LED and the brightness based on where we are in the
        // brightening cycle
        switch ((i/4)%5) {
           case 0:  // red
             colors[idx].red = 200 * brightness/brighteningCycles;
             colors[idx].green = 10 * brightness/brighteningCycles;
             colors[idx].blue = 10 * brightness/brighteningCycles;
             break;
           case 1:  // green
             colors[idx].red = 10 * brightness/brighteningCycles;
             colors[idx].green = 200 * brightness/brighteningCycles;
             colors[idx].blue = 10 * brightness/brighteningCycles;
             break;
           case 2:  // orange
             colors[idx].red = 200 * brightness/brighteningCycles;
             colors[idx].green = 120 * brightness/brighteningCycles;
             colors[idx].blue = 0 * brightness/brighteningCycles;
             break;
           case 3:  // blue
             colors[idx].red = 10 * brightness/brighteningCycles;
             colors[idx].green = 10 * brightness/brighteningCycles;
             colors[idx].blue = 200 * brightness/brighteningCycles;
             break;
           case 4:  // magenta
             colors[idx].red = 200 * brightness/brighteningCycles;
             colors[idx].green = 64 * brightness/brighteningCycles;
             colors[idx].blue = 145 * brightness/brighteningCycles;
             break;
        }
      }
      else {
        // fade the 3/4 of LEDs that we are not currently brightening
        fade(&colors[idx].red, 3);
        fade(&colors[idx].green, 3);
        fade(&colors[idx].blue, 3);
      }
    }
  }

  delay(cycleDelay);
}


// Helper function for adjusting the colors for the BrightTwinkle
// and ColorExplosion patterns.  Odd colors get brighter and even
// colors get dimmer.
void brightTwinkleColorAdjust(unsigned char *color) {
  if (*color == 255) {
    // if reached max brightness, set to an even value to start fade
    *color = 254;
  }
  else if (*color % 2) {
    // if odd, approximately double the brightness
    // you should only use odd values that are of the form 2^n-1,
    // which then gets a new value of 2^(n+1)-1
    // using other odd values will break things
    *color = *color * 2 + 1;
  }
  else if (*color > 0) {
    fade(color, 4);
    if (*color % 2){
      (*color)--;  // if faded color is odd, subtract one to keep it even
    }
  }
}


// Helper function for adjusting the colors for the ColorExplosion
// pattern.  Odd colors get brighter and even colors get dimmer.
// The propChance argument determines the likelihood that neighboring
// LEDs are put into the brightening stage when the central LED color
// is 31 (chance is: 1 - 1/(propChance+1)).  The neighboring LED colors
// are pointed to by leftColor and rightColor (it is not important that
// the leftColor LED actually be on the "left" in your setup).
void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
 unsigned char *leftColor, unsigned char *rightColor) {
  if (*color == 31 && random(propChance+1) != 0) {
    if (leftColor != 0 && *leftColor == 0) {
      *leftColor = 1;  // if left LED exists and color is zero, propagate
    }
    if (rightColor != 0 && *rightColor == 0) {
      *rightColor = 1;  // if right LED exists and color is zero, propagate
    }
  }
  brightTwinkleColorAdjust(color);
}


void colorExplosion(unsigned char noNewBursts, CRGB colors[], int numLeds) {
  // adjust the colors of the first LED
  colorExplosionColorAdjust(&colors[0].red, 9, (unsigned char*)0, &colors[1].red);
  colorExplosionColorAdjust(&colors[0].green, 9, (unsigned char*)0, &colors[1].green);
  colorExplosionColorAdjust(&colors[0].blue, 9, (unsigned char*)0, &colors[1].blue);

  for (int i = 1; i < numLeds - 1; i++) {
    // adjust the colors of second through second-to-last LEDs
    colorExplosionColorAdjust(&colors[i].red, 9, &colors[i-1].red, &colors[i+1].red);
    colorExplosionColorAdjust(&colors[i].green, 9, &colors[i-1].green, &colors[i+1].green);
    colorExplosionColorAdjust(&colors[i].blue, 9, &colors[i-1].blue, &colors[i+1].blue);
  }

  // adjust the colors of the last LED
  colorExplosionColorAdjust(&colors[numLeds-1].red, 9, &colors[numLeds-2].red, (unsigned char*)0);
  colorExplosionColorAdjust(&colors[numLeds-1].green, 9, &colors[numLeds-2].green, (unsigned char*)0);
  colorExplosionColorAdjust(&colors[numLeds-1].blue, 9, &colors[numLeds-2].blue, (unsigned char*)0);

  if (!noNewBursts) {
    // if we are generating new bursts, randomly pick one new LED
    // to light up
    for (int i = 0; i < 1; i++) {
      int j = random(numLeds);  // randomly pick an LED

      // randomly pick a color
      switch(random(7)) {
        // 2/7 chance we will spawn a red burst here (if LED has no red component)
        case 0:
        case 1:
          if (colors[j].red == 0) {
            colors[j].red = 1;
          }
          break;

        // 2/7 chance we will spawn a green burst here (if LED has no green component)
        case 2:
        case 3:
          if (colors[j].green == 0) {
            colors[j].green = 1;
          }
          break;

        // 2/7 chance we will spawn a white burst here (if LED is all off)
        case 4:
        case 5:
          if ((colors[j].red == 0) && (colors[j].green == 0) && (colors[j].blue == 0)) {
            colors[j] = CRGB(1, 1, 1);
          }
          break;

        // 1/7 chance we will spawn a blue burst here (if LED has no blue component)
        case 6:
          if (colors[j].blue == 0) {
            colors[j].blue = 1;
          }
          break;

        default:
          break;
      }
    }
  }
}


void brightTwinkle(
  unsigned char minColor,
  unsigned char numColors,
  unsigned char noNewBursts,
  CRGB colors[],
  int numLeds
) {
  // Note: the colors themselves are used to encode additional state
  // information.  If the color is one less than a power of two
  // (but not 255), the color will get approximately twice as bright.
  // If the color is even, it will fade.  The sequence goes as follows:
  // * Randomly pick an LED.
  // * Set the color(s) you want to flash to 1.
  // * It will automatically grow through 3, 7, 15, 31, 63, 127, 255.
  // * When it reaches 255, it gets set to 254, which starts the fade
  //   (the fade process always keeps the color even).
  for (int i = 0; i < numLeds; i++) {
    brightTwinkleColorAdjust(&colors[i].red);
    brightTwinkleColorAdjust(&colors[i].green);
    brightTwinkleColorAdjust(&colors[i].blue);
  }

  if (!noNewBursts) {
    // if we are generating new twinkles, randomly pick four new LEDs
    // to light up
    for (int i = 0; i < 4; i++) {
      int j = random(numLeds);
      if (colors[j].red == 0 && colors[j].green == 0 && colors[j].blue == 0) {
        // if the LED we picked is not already lit, pick a random
        // color for it and seed it so that it will start getting
        // brighter in that color
        switch (random(numColors) + minColor) {
          case 0:
            colors[j] = CRGB(1, 1, 1);  // white
            break;
          case 1:
            colors[j] = CRGB(1, 0, 0);  // red
            break;
          case 2:
            colors[j] = CRGB(0, 1, 0);  // green
            break;
          case 3:
            colors[j] = CRGB(0, 0, 1);  // blue
            break;
          case 4:
            colors[j] = CRGB(1, 1, 0);  // yellow
            break;
          case 5:
            colors[j] = CRGB(0, 1, 1);  // cyan
            break;
          case 6:
            colors[j] = CRGB(1, 0, 1);  // magenta
            break;
          default:
            colors[j] = CRGB(1, 1, 1);  // white
        }
      }
    }
  }
}


void gradient(CRGB colors[], int numLeds, int loopCount) {
  unsigned int j = 0;

  // populate colors array with full-brightness gradient colors
  // (since the array indices are a function of loopCount, the gradient
  // colors scroll over time)
  while (j < numLeds) {
    // transition from red to green over 8 LEDs
    for (int i = 0; i < 8; i++) {
      if (j >= numLeds){ break; }
      colors[(loopCount/2 + j + numLeds)%numLeds] = CRGB(160 - 20*i, 20*i, (160 - 20*i)*20*i/160);
      j++;
    }
    // transition from green to red over 8 LEDs
    for (int i = 0; i < 8; i++) {
      if (j >= numLeds){ break; }
      colors[(loopCount/2 + j + numLeds)%numLeds] = CRGB(20*i, 160 - 20*i, (160 - 20*i)*20*i/160);
      j++;
    }
  }

  // modify the colors array to overlay the waves of dimness
  // (since the array indices are a function of loopCount, the waves
  // of dimness scroll over time)
  const unsigned char fullDarkLEDs = 10;  // number of LEDs to leave fully off
  const unsigned char fullBrightLEDs = 5;  // number of LEDs to leave fully bright
  const unsigned char cyclePeriod = 14 + fullDarkLEDs + fullBrightLEDs;

  // if numLeds is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (cyclePeriod) and is not
  // smaller than the actual LED count.
  unsigned int extendedLEDCount = (((numLeds-1)/cyclePeriod)+1)*cyclePeriod;

  j = 0;
  while (j < extendedLEDCount) {
    unsigned int idx;

    // progressively dim the LEDs
    for (int i = 1; i < 8; i++) {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= numLeds){ continue; }

      colors[idx].red >>= i;
      colors[idx].green >>= i;
      colors[idx].blue >>= i;
    }

    // turn off these LEDs
    for (int i = 0; i < fullDarkLEDs; i++) {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= numLeds){ continue; }

      colors[idx].red = 0;
      colors[idx].green = 0;
      colors[idx].blue = 0;
    }

    // progressively bring these LEDs back
    for (int i = 0; i < 7; i++) {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= numLeds){ continue; }

      colors[idx].red >>= (7 - i);
      colors[idx].green >>= (7 - i);
      colors[idx].blue >>= (7 - i);
    }

    // skip over these LEDs to leave them at full brightness
    j += fullBrightLEDs;
  }
}


unsigned char collision(CRGB colors[], int numLeds, int loopCount) {
  const unsigned char maxBrightness = 180;  // max brightness for the colors
  const unsigned char numCollisions = 5;  // # of collisions before pattern ends
  static unsigned char state = 0;  // pattern state
  static unsigned int count = 0;  // counter used by pattern

  if (loopCount == 0) {
    state = 0;
  }

  if (state % 3 == 0) {
    // initialization state
    switch (state/3) {
      case 0:  // first collision: red streams
        colors[0] = CRGB(maxBrightness, 0, 0);
        break;
      case 1:  // second collision: green streams
        colors[0] = CRGB(0, maxBrightness, 0);
        break;
      case 2:  // third collision: blue streams
        colors[0] = CRGB(0, 0, maxBrightness);
        break;
      case 3:  // fourth collision: warm white streams
        colors[0] = CRGB(maxBrightness, maxBrightness*4/5, maxBrightness>>3);
        break;
      default:  // fifth collision and beyond: random-color streams
        colors[0] = CRGB(random(maxBrightness), random(maxBrightness), random(maxBrightness));
    }

    // stream is led by two full-white LEDs
    colors[1] = colors[2] = CRGB(255, 255, 255);
    // make other side of the strip a mirror image of this side
    colors[numLeds - 1] = colors[0];
    colors[numLeds - 2] = colors[1];
    colors[numLeds - 3] = colors[2];

    state++;  // advance to next state
    count = 8;  // pick the first value of count that results in a startIdx of 1 (see below)
    return 0;
  }

  if (state % 3 == 1) {
    // stream-generation state; streams accelerate towards each other
    unsigned int startIdx = count*(count + 1) >> 6;
    unsigned int stopIdx = startIdx + (count >> 5);
    count++;
    if (startIdx < (numLeds + 1)/2) {
      // if streams have not crossed the half-way point, keep them growing
      for (int i = 0; i < startIdx-1; i++) {
        // start fading previously generated parts of the stream
        fade(&colors[i].red, 5);
        fade(&colors[i].green, 5);
        fade(&colors[i].blue, 5);
        fade(&colors[numLeds - i - 1].red, 5);
        fade(&colors[numLeds - i - 1].green, 5);
        fade(&colors[numLeds - i - 1].blue, 5);
      }
      for (int i = startIdx; i <= stopIdx; i++) {
        // generate new parts of the stream
        if (i >= (numLeds + 1) / 2) {
          // anything past the halfway point is white
          colors[i] = CRGB(255, 255, 255);
        }
        else {
          colors[i] = colors[i-1];
        }
        // make other side of the strip a mirror image of this side
        colors[numLeds - i - 1] = colors[i];
      }
      // stream is led by two full-white LEDs
      colors[stopIdx + 1] = colors[stopIdx + 2] = CRGB(255, 255, 255);
      // make other side of the strip a mirror image of this side
      colors[numLeds - stopIdx - 2] = colors[stopIdx + 1];
      colors[numLeds - stopIdx - 3] = colors[stopIdx + 2];
    }
    else {
      // streams have crossed the half-way point of the strip;
      // flash the entire strip full-brightness white (ignores maxBrightness limits)
      for (int i = 0; i < numLeds; i++) {
        colors[i] = CRGB(255, 255, 255);
      }
      state++;  // advance to next state
    }
    return 0;
  }

  if (state % 3 == 2) {
    // fade state
    if (colors[0].red == 0 && colors[0].green == 0 && colors[0].blue == 0) {
      // if first LED is fully off, advance to next state
      state++;

      // after numCollisions collisions, this pattern is done
      return state == 3*numCollisions;
    }

    // fade the LEDs at different rates based on the state
    for (int i = 0; i < numLeds; i++) {
      switch (state/3) {
        case 0:  // fade through green
          fade(&colors[i].red, 3);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 2);
          break;
        case 1:  // fade through red
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 3);
          fade(&colors[i].blue, 2);
          break;
        case 2:  // fade through yellow
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 3);
          break;
        case 3:  // fade through blue
          fade(&colors[i].red, 3);
          fade(&colors[i].green, 2);
          fade(&colors[i].blue, 4);
          break;
        default:  // stay white through entire fade
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 4);
      }
    }
  }

  return 0;
}
