#ifndef functions_h
#define functions_h

#include <FastLED.h>
#include <math.h>
#include <stdint.h>
#include <parameters.h>


int getLedIndex32(int x, int y) {
  const int panelWidth = 32;
  const int panelHeight = 8;
  int panel = y / panelHeight;
  int localY = y % panelHeight;
  int baseIndex = panel * panelWidth * panelHeight;
  int panelX = (panel % 2 == scramble1) ? x : (panelWidth - 1 - x);

  if (x % 2 == scramble2)
    return baseIndex + panelX * panelHeight + localY;
  else
    return baseIndex + panelX * panelHeight + (panelHeight - 1 - localY);
}

int getLedIndexBasic(int x, int y) {
  const int panelWidth = 32;
  const int panelHeight = 8;
  int panel = y / panelHeight;
  int localY = y % panelHeight;
  int baseIndex = panel * panelWidth * panelHeight;
  int panelX = (panel % 2 == 0) ? x : (panelWidth - 1 - x);

  if (x % 2 == 0)
    return baseIndex + panelX * panelHeight + localY;
  else
    return baseIndex + panelX * panelHeight + (panelHeight - 1 - localY);
}

uint8_t awesomeSin8(uint16_t input, uint8_t amplitude = 127, uint8_t bias = 128, uint8_t phase = 0, uint8_t frequency = 1) {
    // Apply frequency and phase shift
    uint8_t angle = ((input * frequency) ) + phase;

    // Generate base sine wave (0–255), center is 128
    uint8_t rawSin = sin8(angle);

    // Scale amplitude around center (128), then add bias
    int16_t scaled = ((int16_t(rawSin) - 128) * amplitude) / 128 + bias;

    // Clamp result to 0–255
    if (scaled < 0) return 0;
    if (scaled > 255) return 255;
    return static_cast<uint8_t>(scaled);
}
CRGB adjustHueShiftRGB(const CRGB& color, uint8_t hueShift) {
    // Map hueShift (0–255) to angle (0–255)
    uint8_t sinVal = sin8(hueShift);  // sine wave: 0–255
    uint8_t cosVal = cos8(hueShift);  // cosine wave: 0–255

    // Convert input to float-ish [0–1] using 8-bit math
    uint8_t r = color.r;
    uint8_t g = color.g;
    uint8_t b = color.b;

    // Mix channels based on sine/cosine
    uint8_t newR = (r * cosVal + g * sinVal) >> 8;
    uint8_t newG = (g * cosVal + b * sinVal) >> 8;
    uint8_t newB = (b * cosVal + r * sinVal) >> 8;

    return CRGB(newR, newG, newB);
}

CRGB adjustContrast(const CRGB& color, uint8_t contrast) {
    // contrast: 0 (low) → 128 (neutral) → 255 (high)
    int16_t factor = (int16_t)contrast - 128;  // -128 to +127

    auto apply = [&](uint8_t component) {
        int16_t centered = (int16_t)component - 128; // Center around mid-gray
        int16_t adjusted = centered * (256 + factor * 2) / 256; // Apply contrast scale
        adjusted += 128;  // Shift back
        adjusted = constrain(adjusted, 0, 255);  // Clamp safely
        return (uint8_t)adjusted;
    };

    return CRGB(apply(color.r), apply(color.g), apply(color.b));
}

CRGB adjustSaturationRGB(const CRGB& color, uint8_t saturation) {
    // Compute perceived brightness (grayscale luminance)
    uint8_t gray = (uint8_t)((uint16_t)color.r * 77 + (uint16_t)color.g * 150 + (uint16_t)color.b * 29) >> 8;
    // saturation: 0 = grayscale, 255 = original color
    uint8_t r = lerp8by8(gray, color.r, saturation);
    uint8_t g = lerp8by8(gray, color.g, saturation);
    uint8_t b = lerp8by8(gray, color.b, saturation);
    return CRGB(r, g, b);
}

void generateNoiseFrame() {
    static uint32_t t1 = 0, t2 = 0, t3 = 0; // Z axes for R/G/B noise
    
  
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        uint16_t i = getLedIndex32(x, y);  

    
        // Use high-resolution 16-bit noise and scale it down
        uint8_t valR = inoise16(x * channels[0].xScale, y * channels[0].yScale, t1) >> 8;
        uint8_t valG = inoise16(x * channels[1].xScale, y * channels[1].yScale, t2) >> 8;
        uint8_t valB = inoise16(x * channels[2].xScale, y * channels[2].yScale, t3) >> 8;
  
int sinValR=awesomeSin8(valR,channels[0].amp,channels[0].bias,channels[0].phase,channels[0].sinFreq);
int sinValG=awesomeSin8(valG,channels[1].amp,channels[1].bias,channels[1].phase,channels[1].sinFreq);
int sinValB=awesomeSin8(valB,channels[2].amp,channels[2].bias,channels[2].phase,channels[2].sinFreq);

        // Apply sin curve for visual effect (like your sineTable)
        //CRGB targetColor = CRGB(sin8(valR * sinFreqR), sin8(valG * sinFreqG) ,sin8(valB * sinFreqB));
        CRGB targetColor=CRGB(sinValR,sinValG,sinValB);         
              // Apply low-pass filter: blend previous value with current
      leds[i] = blend(ledsPrev[i], targetColor, smoothing);  // 96 = smoother, lower = slower response      
      ledsPrev[i] = leds[i];  // Store for next frame     
      }
    }   
    t1 += channels[0].tScale;  // evolve time
    t2 += channels[1].tScale;
    t3 += channels[2].tScale;
  
  }
void generateNoiseMask() {
    static uint32_t t1 = 0, t2 = 0, t3 = 0; // Z axes for R/G/B noise
    
  
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        uint16_t i = getLedIndexBasic(x, y);  

    
        // Use high-resolution 16-bit noise and scale it down
  uint8_t valR = inoise16(x * channels[3].xScale, y * channels[3].yScale, t1) >> 8;
        uint8_t valG = inoise16(x * channels[4].xScale, y * channels[4].yScale, t2) >> 8;
        uint8_t valB = inoise16(x * channels[5].xScale, y * channels[5].yScale, t3) >> 8;
  
int sinValR=awesomeSin8(valR,channels[3].amp,channels[3].bias,channels[3].phase,channels[3].sinFreq);
int sinValG=awesomeSin8(valG,channels[4].amp,channels[4].bias,channels[4].phase,channels[4].sinFreq);
int sinValB=awesomeSin8(valB,channels[5].amp,channels[5].bias,channels[5].phase,channels[5].sinFreq);

        // Apply sin curve for visual effect (like your sineTable)
        //CRGB targetColor = CRGB(sin8(valR * sinFreqR), sin8(valG * sinFreqG) ,sin8(valB * sinFreqB));
  CRGB targetColor = CRGB(sinValR, sinValG, sinValB);
  //CRGB targetColor = adjustContrast(rawColor, contrast);  // 128 = neutral
  //CRGB  saturatedColor=adjustSaturationRGB(targetColor,saturation);
              // Apply low-pass filter: blend previous value with current
      mask[i] = blend(maskPrev[i], targetColor, m_smoothing);  // 96 = smoother, lower = slower response
      maskPrev[i] = mask[i];  // Store for next frame     
      }
    }   
    t1 += channels[3].tScale;  // evolve time
    t2 += channels[4].tScale;
    t3 += channels[5].tScale;
  
  }

  void applyColorCorrection(){
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        uint16_t i = getLedIndexBasic(x, y);  
        CRGB hueShifted=adjustHueShiftRGB(leds[i],hueShift);
        CRGB contrasted=adjustContrast(hueShifted,contrast);
        leds[i]=adjustSaturationRGB(contrasted,saturation);

      }
    }
  }

  
#endif

