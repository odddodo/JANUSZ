
#include <hardware_setup.h>
#include <functions.h>
#include <myPalettes.h>
#include <parameters.h>
#include <convolution.h>
#include <talking.h>
#include <steppers.h>


void setup()
{
  initSteppers();
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
    if (dataReady) {
    updateSliderValues();  // Your function
    dataReady = false;
  }
  generateNoiseFrame(0,1);
  applySoftConvolution(emboss, 4, true);
  applySoftConvolution(blur, 3, true);
  
  //applySymmetry1D(pixels, MIRROR_Y);
  shiftPalette();
  blendMultiplePalettes(2);
  update_screen();
  time_counter++;
  backgroundLayer.swapBuffers(false);

  updateSteppers(time_counter);
}
