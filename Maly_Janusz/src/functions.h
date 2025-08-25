#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <hardware_setup.h>
#include <FastLED.h>
#include <parameters.h>
#include <myPalettes.h>
#include <convolution.h>
#include <memory.h>


CRGB pixels[PANEL_RES_X * PANEL_RES_Y];
CRGB mask[PANEL_RES_X * PANEL_RES_Y];

enum SymmetryMode
{
    NONE = 0,
    MIRROR_X = 1,
    MIRROR_Y = 2,
    MIRROR_BOTH = 3
};

inline int index(int x, int y)
{
    return y * PANEL_RES_X + x;
}

void applySymmetry1D(CRGB *pixels, SymmetryMode mode)
{
    for (int y = 0; y < PANEL_RES_Y; y++)
    {
        for (int x = 0; x < PANEL_RES_X; x++)
        {
            int mirrorX = x;
            int mirrorY = y;

            if (mode == MIRROR_X || mode == MIRROR_BOTH)
            {
                mirrorX = PANEL_RES_X - 1 - x;
            }

            if (mode == MIRROR_Y || mode == MIRROR_BOTH)
            {
                mirrorY = PANEL_RES_Y - 1 - y;
            }

            int i = index(x, y);
            int mi = index(mirrorX, mirrorY);

            if (i != mi)
            {
                pixels[mi] = pixels[i];
            }
        }
    }
}

float mapRange(int input, int inMin = 0, int inMax = 255, float outMin = 0.025f, float outMax = 4000.0f)
{
    if (input < inMin)
        input = inMin;
    if (input > inMax)
        input = inMax;

    float scale = (outMax - outMin) / (inMax - inMin);
    return outMin + (input - inMin) * scale;
}
void duplicatePanelContent() {
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            rgb24 c = backgroundLayer.readPixel(x, y);
            backgroundLayer.drawPixel(x, y+64, c);
        }
    }
}
void update_screen()
{
    rgb24 *buffer = backgroundLayer.backBuffer();
    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {
            buffer[PANEL_RES_X * y + x] = (rgb24)pixels[PANEL_RES_X * y + x];
        }
    }
    duplicatePanelContent();
}

void initParams()
{
    Serial.println("initializing parameters...");
    for (int i = 0; i < 6; ++i)
    {
        channels[i].nsclx = 1000.0f;
        channels[i].nscly = 1000.0f;
        channels[i].tscl = 100.0f;
        channels[i].sinscl = 0.025f;
        channels[i].mask = 128.0f;
        channels[i].steepness = 128.0f;
        channels[i].more = 128.0f;
    }
}

void generateNoiseFrame()
{
    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {
            int16_t v = 0;
            int16_t k = 0;
            v = inoise16(x * channels[0].nsclx, y * channels[0].nscly, time_counter * channels[0].tscl);
            k = inoise16(x * channels[1].nsclx, y * channels[1].nscly, time_counter * channels[1].tscl);
            uint8_t v_res = v * channels[0].sinscl;
            uint8_t k_res = k * channels[1].sinscl;
            currentColor = ColorFromPalette(blendedPalette, (((sin8(v_res) * cos8(v_res)) / (1+channels[0].mask)) + ((sin8(k_res) * cos8(k_res)) / (1+ channels[1].mask))) / 2);
            pixels[x + PANEL_RES_X * y] = currentColor;
        }
    }
}

void generateMaskFrame()
{
    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {
            int16_t v = 0;
            int16_t k = 0;
            v = inoise16(x * channels[0].nsclx, y * channels[0].nscly, time_counter * channels[0].tscl);
            k = inoise16(x * channels[1].nsclx, y * channels[1].nscly, time_counter * channels[1].tscl);
            uint8_t v_res = v * channels[0].sinscl;
            uint8_t k_res = k * channels[1].sinscl;
            currentColor = ColorFromPalette(Zebra, (((sin8(v_res) * cos8(v_res)) / 255) + ((sin8(k_res) * cos8(k_res)) / 255)) / 2);
            mask[x + PANEL_RES_X * y] = currentColor;
        }
    }
}

void applyConvolution(float kernel[3][3], int channel, bool stripy)
{
    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {

            int16_t m = 0;
            m = inoise16(x * channels[channel].nsclx, y * channels[channel].nscly, time_counter * channels[channel].tscl);
            uint8_t nVal = (sin8(m * channels[channel].sinscl) * cos8(m * channels[channel].sinscl)) / 255;
            if (stripy)
            {
                if (nVal < channels[channel].mask)
                {
                    CRGB ccolor = convolution(x, y, kernel, pixels);
                    int loc = x + PANEL_RES_X * y;
                    pixels[loc] = ccolor;
                }
            }
            else
            {
                if (m < channels[channel].mask)
                {
                    CRGB ccolor = convolution(x, y, kernel, pixels);
                    int loc = x + PANEL_RES_X * y;
                    pixels[loc] = ccolor;
                }
            }
        }
    }
}

void applySoftConvolution(float kernel[3][3], int channel, bool stripy)
{
    auto& ch = channels[channel];  // Alias for clarity

    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {
            int16_t m = inoise16(
                x * ch.nsclx,
                y * ch.nscly,
                time_counter * ch.tscl
            );

            // Scale m to 0–255 range
            uint8_t m8 = m >> 8;

            // Generate blend weight (0–255) based on distance to mask
            uint8_t blendAmount;

            if (stripy)
            {
                uint8_t nVal = (sin8(m8 * ch.sinscl) * cos8(m8 * ch.sinscl)) / 255;
                int16_t distance = (int16_t)ch.mask - nVal;
                blendAmount = qsub8(255, abs8(distance) * ch.steepness); // Steepness control (x4)
            }
            else
            {
                int16_t distance = (int16_t)ch.mask - m8;
                blendAmount = qsub8(255, abs8(distance) * ch.steepness); // Adjust x4 for steepness
            }

            // Clamp blendAmount to 0–255
            blendAmount = constrain(blendAmount, 0, 255);

            int loc = x + PANEL_RES_X * y;

            // Only blend if it's visible enough
            if (blendAmount > 0)
            {
                CRGB original = pixels[loc];
                CRGB filtered = convolution(x, y, kernel, pixels);
                pixels[loc] = blend(original, filtered, blendAmount);
            }
        }
    }
}
void updateSliderValues()
{
    for (int i = 0; i < 6; ++i)
    {
        int baseIndex = i * 7;
        channels[i].nsclx = mapRange(sliderValues[baseIndex + 0], 0, 255, 50.0f, 1000.0f);
        channels[i].nscly = mapRange(sliderValues[baseIndex + 1], 0, 255, 50.0f, 1000.0f);
        channels[i].tscl = mapRange(sliderValues[baseIndex + 2], 0, 255, 0.1f, 500.0f);
        channels[i].sinscl = mapRange(sliderValues[baseIndex + 3], 0, 255, 0.01f, 0.1f);
        channels[i].mask = mapRange(sliderValues[baseIndex + 4], 0, 255, 0, 255);
        channels[i].steepness = mapRange(sliderValues[baseIndex + 5], 0, 255, 0, 255);
        channels[i].more = mapRange(sliderValues[baseIndex + 6], 0, 255, 0, 255);
    }
    //Serial.println("updating!");

}
void rememberSettings(){
    saveArrayToSD(sliderValues,NUMSLIDERS);
    Serial.println("saving!");
}
void recallSettings(){
      if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
    return;
  }

  Serial.println("SD card ready.");

  // Try to load data
  if (loadArrayFromSD(sliderValues, NUMSLIDERS)) {
    updateSliderValues();
    Serial.println("recalling!");
        for (int i = 0; i < NUMSLIDERS; i++) {
      Serial.println(sliderValues[i]);
    }
  }
  else{
    for (int i = 0; i < NUMSLIDERS; i++) {
      Serial.println("failed recalling!");
    }
//updateSliderValues();
    // Save initialized array
    //saveArrayToSD(sliderValues, NUM_SLIDERS);
  }
}



#endif