#ifndef PARAMETERS_H
#define PARAMETERS_H
//binarynonbinary
#include <math.h>
#include <stdint.h>

#define I2C_SLAVE_ADDR 0x08  // Must match master's address

const int NUMSLIDERS = 30;
uint8_t sliderValues[NUMSLIDERS]; // Local buffer to hold incoming slider values

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

ChannelSettings channels[5]; 

uint8_t blendIndex = 0;

#endif