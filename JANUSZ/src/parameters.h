#ifndef parameters_h
#define parameters_h

#include <FastLED.h>

#define DISPLAY_WIDTH 32
#define DISPLAY_HEIGHT 32
#define NUM_PIXELS (DISPLAY_WIDTH * DISPLAY_HEIGHT)
#define LED_PIN 19
#define COLOR_ORDER RBG
#define CHIPSET WS2812B
#define BRIGHTNESS 128

#define I2C_SLAVE_ADDR 0x08  // Must match master's address

const int NUM_SLIDERS = 45;
uint8_t sliderValues[NUM_SLIDERS];

struct ChannelSettings {
   int sinFreq;
   int amp;
   int bias;
   int phase;
   int xScale;
   int yScale;
   int tScale;
};

ChannelSettings channels[6]; // 0: R, 1: G, 2: B, 3: NR, 4: NG, 5: NB

CRGB leds[NUM_PIXELS];
CRGB ledsPrev[NUM_PIXELS];  // buffer to hold previous smoothed values

CRGB mask[NUM_PIXELS];
CRGB maskPrev[NUM_PIXELS];

int  smoothing=96;

uint8_t  scramble1=0;
uint8_t  scramble2=0;

uint8_t  m_smoothing=96;

typedef uint8_t (*ChannelSelector)(const CRGB&);
uint8_t redChannel(const CRGB& c)   { return c.r; }
uint8_t greenChannel(const CRGB& c) { return c.g; }
uint8_t blueChannel(const CRGB& c)  { return c.b; }

#endif