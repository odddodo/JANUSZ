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

CRGB leds[NUM_PIXELS];
CRGB ledsPrev[NUM_PIXELS];  // buffer to hold previous smoothed values

CRGB mask[NUM_PIXELS];
CRGB maskPrev[NUM_PIXELS];

uint8_t sinFreqR = 3;
uint8_t sinFreqG = 15;
uint8_t sinFreqB = 7;

uint8_t amp_R=255;
uint8_t amp_G=255;
uint8_t amp_B=255;

uint8_t bias_R =0;
uint8_t bias_G =0;
uint8_t bias_B =0;

uint8_t phase_R=0;
uint8_t phase_G=0;
uint8_t phase_B=0;

int xScaleR= 5000, yScaleR= 5000;
int xScaleG = 5000, yScaleG= 5000;
int xScaleB = 5000, yScaleB = 5000;

int  tScaleR=10;
int  tScaleG=30;
int tScaleB=10;

int  smoothing=96;

uint8_t  scramble1=0;
uint8_t  scramble2=0;

//mask parameters:

uint8_t m_sinFreqR = 2;
uint8_t m_sinFreqG = 5;
uint8_t m_sinFreqB = 7;

uint8_t m_amp_R=128;
uint8_t m_amp_G=128;
uint8_t m_amp_B=128;

uint8_t m_bias_R =128;
uint8_t m_bias_G =128;
uint8_t m_bias_B =128;

uint8_t m_phase_R=0;
uint8_t m_phase_G=0;
uint8_t m_phase_B=0;

int m_xScaleR= 3000, m_yScaleR= 3000;
int m_xScaleG = 5000, m_yScaleG= 5000;
int m_xScaleB = 7000, m_yScaleB = 7000;

int  m_tScaleR=64;
int  m_tScaleG=64;
int  m_tScaleB=64;

uint8_t  m_smoothing=96;

typedef uint8_t (*ChannelSelector)(const CRGB&);
uint8_t redChannel(const CRGB& c)   { return c.r; }
uint8_t greenChannel(const CRGB& c) { return c.g; }
uint8_t blueChannel(const CRGB& c)  { return c.b; }

#endif