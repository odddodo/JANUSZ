
#include <hardware_setup.h>
#include <functions.h>
#include <myPalettes.h>
#include <parameters.h>
#include <convolution.h>
#include <talking.h>

void setup()
{
  delay(1000);
  initMemory();
  delay(1000);
  initSliderValues();
  delay(1000);
  initTalking();
  recallSettings();
  delay(1000);
  matrix.addLayer(&backgroundLayer);
  matrix.begin();
  delay(1000);
  backgroundLayer.setBrightness(255);
}

void loop()
{

  while (backgroundLayer.isSwapPending());
  
  generateNoiseFrame();
  applySoftConvolution(darken, 2, true);
  applySoftConvolution(blur, 3, false);
  applySoftConvolution(emboss, 4, true);
  applySoftConvolution(blur, 5, false);
  applySymmetry1D(pixels, MIRROR_X);
  blendMultiplePalettes(channels[0].mask);
  update_screen();

  time_counter++;
  backgroundLayer.swapBuffers(false);
}
