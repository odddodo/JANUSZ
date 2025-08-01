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

void applyMaskedConvolution(const int8_t kernel[3][3], CRGB* mask, ChannelSelector selectChannel, int divisor = 1, int bias = 0) {
  // Copy current LED state to source buffer
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      source[y][x] = leds[getLedIndexBasic(x, y)];
    }
  }

  // Apply convolution to the entire frame with clamped edges
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      int idx = getLedIndexBasic(x, y);
      int blendAmount = selectChannel(mask[idx]);

      int sumR = 0, sumG = 0, sumB = 0;

      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          // Clamp neighbor coordinates to avoid out-of-bounds
          int ny = constrain(y + ky, 0, DISPLAY_HEIGHT - 1);
          int nx = constrain(x + kx, 0, DISPLAY_WIDTH - 1);
          CRGB neighbor = source[ny][nx];

          int kVal = kernel[ky + 1][kx + 1];
          sumR += neighbor.r * kVal;
          sumG += neighbor.g * kVal;
          sumB += neighbor.b * kVal;
        }
      }

      // Normalize and apply bias
      CRGB convolved;
      convolved.r = CLAMP((sumR / divisor) + bias);
      convolved.g = CLAMP((sumG / divisor) + bias);
      convolved.b = CLAMP((sumB / divisor) + bias);

      // Blend with original pixel based on mask
      temp[y][x] = blend(source[y][x], convolved, blendAmount);
    }
  }

  // Write result back to LED array
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      leds[getLedIndexBasic(x, y)] = temp[y][x];
    }
  }
}


#endif