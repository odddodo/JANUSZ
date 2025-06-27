#ifndef PARAMETERS_H
#define PARAMETERS_H
//binarynonbinary
#include <math.h>
#include <stdint.h>

#define I2C_SLAVE_ADDR 0x08  // Must match master's address

const int NUM_SLIDERS = 42;
uint8_t sliderValues[NUM_SLIDERS];

uint16_t time_counter = 0;

struct ChannelSettings {
float nsclx;
float nscly;
float tscl;
float sinscl;
float mask;
float steepness;
float more;
};

ChannelSettings channels[6]; 

uint8_t blendIndex = 0;

#endif