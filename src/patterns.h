#include "FastLED.h"

/*
  This function applies a random walk to val by increasing or
  decreasing it by changeAmount or by leaving it unchanged.
  val is a pointer to the byte to be randomly changed.
  The new value of val will always be within [0, maxVal].
  A walk direction of 0 decreases val and a walk direction of 1
  increases val.  The directions argument specifies the number of
  possible walk directions to choose from, so when directions is 1, val
  will always decrease; when directions is 2, val will have a 50% chance
  of increasing and a 50% chance of decreasing; when directions is 3,
  val has an equal chance of increasing, decreasing, or staying the same.
*/
void randomWalk(
  unsigned char *val,
  unsigned char maxVal,
  unsigned char changeAmount,
  unsigned char directions
);

/*
  This function fades val by decreasing it by an amount proportional
  to its current value.  The fadeTime argument determines the
  how quickly the value fades.  The new value of val will be:
  val = val - val*2^(-fadeTime)
  So a smaller fadeTime value leads to a quicker fade.
  If val is greater than zero, val will always be decreased by
  at least 1.
  val is a pointer to the byte to be faded.
*/
void fade(unsigned char *val, unsigned char fadeTime);

/*
  ***** PATTERN WarmWhiteShimmer *****
  This function randomly increases or decreases the brightness of the
  even red LEDs by changeAmount, capped at maxBrightness.  The green
  and blue LED values are set proportional to the red value so that
  the LED color is warm white.  Each odd LED is set to a quarter the
  brightness of the preceding even LEDs.  The dimOnly argument
  disables the random increase option when it is true, causing
  all the LEDs to get dimmer by changeAmount; this can be used for a
  fade-out effect.
*/
void warmWhiteShimmer(unsigned char dimOnly, CRGB colors[], int numLeds);

/*
  ***** PATTERN RandomColorWalk *****
  This function randomly changes the color of every seventh LED by
  randomly increasing or decreasing the red, green, and blue components
  by changeAmount (capped at maxBrightness) or leaving them unchanged.
  The two preceding and following LEDs are set to progressively dimmer
  versions of the central color.  The initializeColors argument
  determines how the colors are initialized:
    0: randomly walk the existing colors
    1: set the LEDs to alternating red and green segments
    2: set the LEDs to random colors
  When true, the dimOnly argument changes the random walk into a 100%
  chance of LEDs getting dimmer by changeAmount; this can be used for
  a fade-out effect.
*/
void randomColorWalk(
  unsigned char initializeColors,
  unsigned char dimOnly,
  CRGB colors[],
  int numLeds
);

/*
  ***** PATTERN TraditionalColors *****
  This function creates a repeating patern of traditional Christmas
  light colors: red, green, orange, blue, magenta.
  Every fourth LED is colored, and the pattern slowly moves by fading
  out the current set of lit LEDs while gradually brightening a new
  set shifted over one LED.
*/
void traditionalColors(
  CRGB colors[],
  int numLeds,
  unsigned int loopCount,
  unsigned int cycleDelay
);

/*
  ***** PATTERN ColorExplosion *****
  This function creates bursts of expanding, overlapping colors by
  randomly picking LEDs to brighten and then fade away.  As these LEDs
  brighten, they have a chance to trigger the same process in
  neighboring LEDs.  The color of the burst is randomly chosen from
  among red, green, blue, and white.  If a red burst meets a green
  burst, for example, the overlapping portion will be a shade of yellow
  or orange.
  When true, the noNewBursts argument changes prevents the generation
  of new bursts; this can be used for a fade-out effect.
  This function uses a very similar algorithm to the BrightTwinkle
  pattern.  The main difference is that the random twinkling LEDs of
  the BrightTwinkle pattern do not propagate to neighboring LEDs.
*/
void colorExplosion(unsigned char noNewBursts, CRGB colors[], int numLeds);

/*
  ***** PATTERN BrightTwinkle *****
  This function creates a sparkling/twinkling effect by randomly
  picking LEDs to brighten and then fade away.  Possible colors are:
    white, red, green, blue, yellow, cyan, and magenta
  numColors is the number of colors to generate, and minColor
  indicates the starting point (white is 0, red is 1, ..., and
  magenta is 6), so colors generated are all of those from minColor
  to minColor+numColors-1.  For example, calling brightTwinkle(2, 2, 0)
  will produce green and blue twinkles only.
  When true, the noNewBursts argument changes prevents the generation
  of new twinkles; this can be used for a fade-out effect.
  This function uses a very similar algorithm to the ColorExplosion
  pattern.  The main difference is that the random twinkling LEDs of
  this BrightTwinkle pattern do not propagate to neighboring LEDs.
*/
void brightTwinkle(
  unsigned char minColor,
  unsigned char numColors,
  unsigned char noNewBursts,
  CRGB colors[],
  int numLeds
);

/*
  ***** PATTERN Gradient *****
  This function creates a scrolling color gradient that smoothly
  transforms from red to white to green back to white back to red.
  This pattern is overlaid with waves of brightness and dimness that
  scroll at twice the speed of the color gradient.
*/
void gradient(CRGB colors[], int numLeds, int loopCount);

/*
  ***** PATTERN Collision *****
  This function spawns streams of color from each end of the strip
  that collide, at which point the entire strip flashes bright white
  briefly and then fades.  Unlike the other patterns, this function
  maintains a lot of complicated state data and tells the main loop
  when it is done by returning 1 (a return value of 0 means it is
  still in progress).
*/
unsigned char collision(CRGB colors[], int numLeds, int loopCount);
