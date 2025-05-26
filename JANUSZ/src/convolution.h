#ifndef convolution_h
#define convolution_h

#include <FastLED.h>
#include <parameters.h>
#include <functions.h>

#define CLAMP(v) ((v) < 0 ? 0 : ((v) > 255 ? 255 : (v)))

const int8_t blurKernel[3][3] = {
  {1, 1, 1},
  {1, 1, 1},
  {1, 1, 1}
};

const int8_t gaussianKernel[3][3] = {
  {1, 2, 1},
  {2, 4, 2},
  {1, 2, 1}
};

const int8_t edgeKernel[3][3] = {
  {-1, -1, -1},
  {-1,  8, -1},
  {-1, -1, -1}
};

const int8_t strongSharpenKernel[3][3] = {
  { 0, -1,  0},
  {-1,  9, -1},
  { 0, -1,  0}
};

const int8_t radicalEdgeKernel[3][3] = {
  {-2, -1,  0},
  {-1,  6,  1},
  { 0,  1,  2}
};//div:1, bias:128

const int8_t negativeEmbossKernel[3][3] = {
  { 2,  1,  0},
  { 1, -1, -1},
  { 0, -1, -2}
};
// Divisor: 1
// Bias: 128

const int8_t inversionKernel[3][3] = {
  { 1,  1,  1},
  { 1, -9,  1},
  { 1,  1,  1}
};
// Divisor: 1
// Bias: 128 (to make dark results visible)

const int8_t ghostKernel[3][3] = {
  {-3, 0, 3},
  {-6, 0, 6},
  {-3, 0, 3}
};
// Divisor: 1 or 3
// Bias: Optional

    CRGB temp[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    CRGB source[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  void applyMaskedConvolution(const int8_t kernel[3][3],  CRGB* mask, ChannelSelector selectChannel, int divisor = 1, int bias = 0) {

  
    // Copy current LED state
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        source[y][x] = leds[getLedIndexBasic(x, y)];
      }
    }
  
    // Apply convolution
    for (int y = 0; y < DISPLAY_HEIGHT ; y++) {
      for (int x = 0; x < DISPLAY_WIDTH ; x++) {
        int idx = getLedIndexBasic(x, y);
        int blendAmount = selectChannel(mask[idx]);
  
        int sumR = 0, sumG = 0, sumB = 0;
        for (int ky = -1; ky <= 1; ky++) {
          for (int kx = -1; kx <= 1; kx++) {
            CRGB neighbor = source[y + ky][x + kx];
            int kVal = kernel[ky + 1][kx + 1];
            sumR += neighbor.r * kVal;
            sumG += neighbor.g * kVal;
            sumB += neighbor.b * kVal;
          }
        }
  
        CRGB convolved;
        convolved.r = CLAMP((sumR / divisor) + bias);
        convolved.g = CLAMP((sumG / divisor) + bias);
        convolved.b = CLAMP((sumB / divisor) + bias);
  
        // Blend based on mask intensity
        temp[y][x] = blend(source[y][x], convolved, blendAmount);
      }
    }
  
    // Copy edges unchanged
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      temp[0][x] = source[0][x];
      temp[DISPLAY_HEIGHT - 1][x] = source[DISPLAY_HEIGHT - 1][x];
    }
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      temp[y][0] = source[y][0];
      temp[y][DISPLAY_WIDTH - 1] = source[y][DISPLAY_WIDTH - 1];
    }
  
    // Write result back to LED array
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        leds[getLedIndexBasic(x, y)] = temp[y][x];
      }
    }
  }

#endif