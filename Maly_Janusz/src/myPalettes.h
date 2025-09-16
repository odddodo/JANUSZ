#ifndef MYPALETTES_H
#define MYPALETTES_H

#include "FastLED.h"
#include "colorutils.h"
#include "colorpalettes.h"

CRGB currentColor, otherColor;
uint8_t phaseOffset = 0; // 0–255
uint8_t hueShiftINDX = 0; // 0–255

// Define XGA-inspired colors
#define XGA_CYAN CRGB(0, 255, 255)    // Bright cyan
#define XGA_MAGENTA CRGB(255, 0, 255) // Bright magenta
#define XGA_YELLOW CRGB(255, 255, 0)  // Bright yellow
#define XGA_BLACK CRGB(0, 0, 0)       // True black
#define XGA_WHITE CRGB(255, 255, 255) // True white

const uint8_t numPalettes = 7;


extern const TProgmemRGBPalette16 Zebra FL_PROGMEM =
    {
        CRGB::Black,
        CRGB::GhostWhite,
        CRGB::DarkGrey,
        CRGB::Black,
        CRGB::GhostWhite,
        CRGB::DarkGrey,
        CRGB::Black,
        CRGB::GhostWhite,
CRGB::Black,
        CRGB::DarkGrey,
CRGB::Black,
        CRGB::DarkGrey,
CRGB::Black,
        CRGB::DarkGrey,
CRGB::Black,
        CRGB::DarkGrey,
  
};
const CRGBPalette16 XGAPalette = CRGBPalette16(
    XGA_CYAN, XGA_CYAN, XGA_CYAN, XGA_CYAN,
    XGA_MAGENTA, XGA_MAGENTA, XGA_MAGENTA, XGA_MAGENTA,
    XGA_YELLOW, XGA_YELLOW, XGA_YELLOW, XGA_YELLOW,
    XGA_WHITE, XGA_WHITE, XGA_BLACK, XGA_BLACK);

extern const TProgmemRGBPalette16 HugMeColors FL_PROGMEM =
    {
        CRGB::HotPink,
        CRGB::Olive,
        CRGB::YellowGreen,
        CRGB::Black,
        CRGB::DarkSalmon,
        CRGB::Olive,
        CRGB::DarkBlue,
        CRGB::Black,
                CRGB::DarkOrchid,
        CRGB::Olive,
        CRGB::DarkGoldenrod,
        CRGB::Black,
        CRGB::DarkMagenta,
        CRGB::DarkKhaki,
        CRGB::RoyalBlue,
        CRGB::Black,

};
extern const TProgmemRGBPalette16 Hello FL_PROGMEM =
    {
        CRGB::Red,
        CRGB::Orange,
        CRGB::Yellow,
        CRGB::DarkGreen,
        CRGB::DarkBlue,
        CRGB::Purple,
                CRGB::Red,
        CRGB::Orange,
        CRGB::Yellow,
        CRGB::DarkGreen,
        CRGB::DarkBlue,
        CRGB::Purple,
                CRGB::Red,
        CRGB::Orange,
        CRGB::Yellow,
        CRGB::DarkGreen,



};


extern const TProgmemRGBPalette16 XGAColors FL_PROGMEM =
    {
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan,
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan,  
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan,
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan, 

};

extern const TProgmemRGBPalette16 Schmussn FL_PROGMEM =
    {
        CRGB::Black,
        CRGB::Chocolate,
        CRGB::MediumSeaGreen,
        CRGB::OrangeRed,
        CRGB::LightGoldenrodYellow,
        CRGB::Chocolate,
        CRGB::MediumSeaGreen,
        CRGB::OrangeRed ,
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan,
        CRGB::Black,
        CRGB::Yellow,
        CRGB::Magenta,
        CRGB::Cyan, 

};
extern const TProgmemRGBPalette16 Marylin FL_PROGMEM =
    {
        CRGB::Black,
        CRGB::IndianRed,
        CRGB::RoyalBlue,
        CRGB::Yellow,
        CRGB::Black,
        CRGB::IndianRed,
        CRGB::RoyalBlue,
        CRGB::Yellow,
        CRGB::Black,
        CRGB::Chocolate,
        CRGB::MediumSeaGreen,
        CRGB::OrangeRed,
        CRGB::LightGoldenrodYellow,
        CRGB::Chocolate,
        CRGB::MediumSeaGreen,
        CRGB::OrangeRed ,

};
extern const TProgmemRGBPalette16 Trans FL_PROGMEM =
    {
        CRGB::CadetBlue,
        CRGB::HotPink,
        CRGB::White,
        CRGB::HotPink,
        CRGB::CadetBlue,
         CRGB::HotPink,
        CRGB::White,
        CRGB::HotPink,
                CRGB::CadetBlue,
        CRGB::HotPink,
        CRGB::White,
        CRGB::HotPink,
        CRGB::CadetBlue,
         CRGB::HotPink,
        CRGB::White,
        CRGB::HotPink,

};
extern const TProgmemRGBPalette16 Italy FL_PROGMEM =
    {
       
        CRGB::GhostWhite,
        CRGB::Red,
        CRGB::Green,
        CRGB::Black,
           CRGB::GhostWhite,
        CRGB::Red,
        CRGB::Green,
        CRGB::Black,
   
       
        CRGB::WhiteSmoke,
       CRGB::Blue,
        CRGB::WhiteSmoke,
       CRGB::Blue,
        CRGB::WhiteSmoke,
       CRGB::Blue,
        CRGB::WhiteSmoke,
        CRGB::Blue,

};
extern const TProgmemRGBPalette16 Smoothie FL_PROGMEM =
    {
        CRGB::DarkOrchid,
        CRGB::Olive,
        CRGB::DarkGoldenrod,
        CRGB::Black,
        CRGB::DarkMagenta,
        CRGB::DarkKhaki,
        CRGB::RoyalBlue,
        CRGB::Black,
        CRGB::DarkOrchid,
        CRGB::Olive,
        CRGB::DarkGoldenrod,
        CRGB::Black,
        CRGB::DarkMagenta,
        CRGB::DarkKhaki,
        CRGB::RoyalBlue,
        CRGB::Black,
        };
        extern const TProgmemRGBPalette16 Arctic FL_PROGMEM =
    {
                CRGB::White,
        CRGB::Blue,
        CRGB::Red,
                     CRGB::White,
        CRGB::Blue,
        CRGB::Red,
        CRGB::DarkBlue,
        CRGB::Gold,
        CRGB::DarkBlue,
        CRGB::Gold,
                CRGB::DarkBlue,
        CRGB::Gold,
                   CRGB::White,
             CRGB::Blue,
        CRGB::Red,
          
                     CRGB::Gold,
        
        };

CRGBPalette16 palettes[numPalettes] = {
    Zebra,
    Hello,
    Smoothie,
    XGAColors,   
    Arctic,
    Italy,
    HugMeColors,
    };


CRGBPalette16 blendedPalette;

CRGB shiftHue(const CRGB& color, uint8_t hueShift) {
    CHSV hsv=  rgb2hsv_approximate(color);  // FastLED built-in
    hsv.hue += hueShift; 
    CRGB c;
    hsv2rgb_rainbow(hsv,c)  ;          // Wraps automatically
    return c;
}
CRGBPalette16 shiftedPalette;
void shiftPalette(){

for (int i = 0; i < 16; i++) {
    shiftedPalette[i] = shiftHue(blendedPalette[i], hueShiftINDX);
}
}

void blendMultiplePalettes(int channel) {

auto& ch = channels[channel];  // Alias for clarity
        phaseOffset=ch.steepness;
     hueShiftINDX=ch.sinscl;
    
        uint8_t blendIndex = ch.mask;

    if (numPalettes < 2) {
        // Not enough palettes to blend
        blendedPalette = palettes[0];
        return;
    }

    // Scale 0–255 across (numPalettes - 1) segments
    float position = (blendIndex / 255.0f) * (numPalettes - 1);

    uint8_t indexA = floor(position);
    uint8_t indexB = min(indexA + 1, numPalettes - 1);

    float blendAmount = position - indexA; // Fractional part, 0.0–1.0
    uint8_t blendAmount8 = (uint8_t)(blendAmount * 255); // Convert to 0–255

    for (int i = 0; i < 16; i++) {
        CRGB colorA = palettes[indexA][i];
        CRGB colorB = palettes[indexB][i];
        blendedPalette[i] = blend(colorA, colorB, blendAmount8);
    }
}


CRGBPalette16 currentPalette = Schmussn;
CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND)
{
  // Apply phase offset (wrap automatically since uint8_t rolls over)

  uint8_t shiftedIndex = index + phaseOffset;  
  return  ColorFromPalette(shiftedPalette, shiftedIndex, brightness, blendType);
}
#endif