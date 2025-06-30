#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

#include <MatrixHardware_Teensy4_ShieldV5.h> // SmartLED Shield for Teensy 4 (V5)
#include <SmartMatrix.h>
#include <FastLED.h>

#define ENABLE_HUB75_REFRESH 1

#define PANEL_RES_X 64
#define PANEL_RES_Y 64

#define kPanelsChain 2

#define COLOR_DEPTH 24 // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)

#if (ENABLE_HUB75_REFRESH == 1)
const uint16_t kMatrixWidth = PANEL_RES_X;               // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = PANEL_RES_Y*kPanelsChain;              // Set to the height of your display
const uint8_t kRefreshDepth = 36;                        // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;                        // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_64ROW_MOD32SCAN;                            // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_BOTTOM_TO_TOP_STACKING); // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);


#endif

#endif