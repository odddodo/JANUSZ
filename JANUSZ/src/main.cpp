#include <Arduino.h>
#include <convolution.h>
#include <talking.h>

void setup() {
Serial.begin(115200);
initSliderValues();
initTalking();
  
    Serial.println("I2C Slave Ready");

FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_PIXELS);
//FastLED.setCorrection(TypicalLEDStrip);  // or UncorrectedColor
FastLED.setBrightness(BRIGHTNESS);
}

void loop() {

generateNoiseFrame();
generateNoiseMask();

//applyMaskedConvolution(ghostKernel, mask,blueChannel, 3);
applyMaskedConvolution(inversionKernel, mask,blueChannel, 1,128);
applyMaskedConvolution(radicalEdgeKernel, mask,greenChannel, 1, 128);
applyMaskedConvolution(blurKernel,mask,redChannel, 9);
//applyMaskedConvolution(gaussianKernel,mask,redChannel, 16);

applyColorCorrection();


FastLED.show();

}