#ifndef talking_h
#define talking_h
#include <parameters.h>
#include <Wire.h>
#include <functions.h>



void initSliderValues()
{
    for (int i = 0; i < NUM_SLIDERS; i++)
    {
        sliderValues[i] = 128;
    }
    updateSliderValues();
}
void receiveEvent(int numBytes)
{
    int i = 0;
    while (Wire2.available() && i < NUM_SLIDERS)
    {
        sliderValues[i++] = Wire2.read();
    }
    updateSliderValues();
   rememberSettings();
    Serial.println("gotSomething!");
}
void requestEvent()
{
    Wire2.write(sliderValues, NUM_SLIDERS);
    Serial.println("requesting!");
}

void initTalking()
{
    Serial.begin(115200);
    Wire2.begin(I2C_SLAVE_ADDR); // Initialize as I2C slave
    Wire2.setSCL(25);
    Wire2.setSDA(24);
    Wire2.onReceive(receiveEvent); // Register receive handler
    Wire2.onRequest(requestEvent);
    Serial.println("alive");
}

#endif