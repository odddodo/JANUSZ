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

// Build a 16-color palette using CMYK + W in a repeated pattern
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
        CRGB::Black

};
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
        CRGB::DarkGrey,
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
        CRGB::OrangeRed,
        CRGB::Gold,
        CRGB::Chocolate,
        CRGB::MediumSeaGreen,
        CRGB::OrangeRed,
        CRGB::LightYellow,

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

};

CRGBPalette16 palettes[5] = {
    Zebra,
    FuckMeHardColors,
    Schmussn,
    Marylin,
    XGAColors};

CRGBPalette16 blendedPalette;

void blendMultiplePalettes(uint8_t blendIndex)
{
  // There are 4 blend zones between 5 palettes
  const uint8_t numPalettes = 5;
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