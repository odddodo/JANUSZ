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


applyMaskedConvolution(inversionKernel, mask,blueChannel, 1,128);
applyMaskedConvolution(radicalEdgeKernel, mask,greenChannel, 1, 128);
applyMaskedConvolution(blurKernel,mask,redChannel, 9);


applyColorCorrection();


FastLED.show();

}