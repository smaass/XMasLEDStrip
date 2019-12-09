#include <Arduino.h>

// #define LED_TYPE APA102
#define LED_TYPE SK9822
#define COLOR_ORDER BGR

// ATMega328P Hardware SPI: data 11, clock 13
// https://github.com/FastLED/FastLED/wiki/SPI-Hardware-or-Bit-banging
const uint8_t DATA_PIN = 11;
const uint8_t CLOCK_PIN = 13;

const int NUM_LEDS = 60;
const uint8_t BRIGHTNESS = 200;
const uint8_t FRAMES_PER_SECOND = 120;

const uint8_t NEXT_PATTERN_BUTTON_PIN = 2; // button between this pin and ground
const uint8_t AUTOCYCLE_SWITCH_PIN = 3; // switch between this pin and ground
