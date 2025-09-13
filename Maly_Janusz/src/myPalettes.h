#ifndef MYPALETTES_H
#define MYPALETTES_H

#include "FastLED.h"
#include "colorutils.h"
#include "colorpalettes.h"

CRGB currentColor, otherColor;

// Define XGA-inspired colors
#define XGA_CYAN CRGB(0, 255, 255)    // Bright cyan
#define XGA_MAGENTA CRGB(255, 0, 255) // Bright magenta
#define XGA_YELLOW CRGB(255, 255, 0)  // Bright yellow
#define XGA_BLACK CRGB(0, 0, 0)       // True black
#define XGA_WHITE CRGB(255, 255, 255) // True white

const uint8_t numPalettes = 9;
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

extern const TProgmemRGBPalette16 FuckMeHardColors FL_PROGMEM =
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
    Schmussn,
    Marylin,
    XGAColors,
    Arctic,
    Italy,
    FuckMeHardColors,
    };

CRGBPalette16 blendedPalette;

void blendMultiplePalettes(int channel)
{
    uint8_t blendIndex=channels[channel].mask;

  
  const uint8_t blendRange = 256 / (numPalettes - 1); // 64 steps

  uint8_t indexA = blendIndex / blendRange;
  uint8_t indexB = min(indexA + 1, numPalettes - 1);   // Prevent overflow
  uint8_t blendAmount = (blendIndex % blendRange) * 4; // Scale to 0–255

  for (int i = 0; i < 16; i++)
  {
    CRGB colorA = palettes[indexA][i];
    CRGB colorB = palettes[indexB][i];
    blendedPalette[i] = blend(colorA, colorB, blendAmount);
  }
}

CRGBPalette16 currentPalette = Schmussn;
CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND)
{
  return ColorFromPalette(blendedPalette, index, brightness, blendType);
}
#endif