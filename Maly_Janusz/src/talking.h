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
#define SAVE_COMMAND_BYTE 0xFF
#define LOAD_COMMAND_BYTE 0xFE

void receiveEvent(int numBytes)
{
    int i = 0;

    // Read up to NUM_SLIDERS bytes into sliderValues
    while (Wire2.available() && i < NUM_SLIDERS)
    {
        sliderValues[i++] = Wire2.read();
    }

    // If there's an extra byte, check if it's the save command
    if (Wire2.available())
    {
        uint8_t command = Wire2.read();
        if (command == SAVE_COMMAND_BYTE)
        {
            Serial.println("Save command received!");
            rememberSettings();  // Implement this function to write to SD
        }
        else if (command == SAVE_COMMAND_BYTE)
        {
            Serial.println("load command received!");
            recallSettings();  // Implement this function to write to SD
        }
        else
        {
            Serial.printf("Unknown extra command byte: 0x%02X\n", command);
        }
    }

    updateSliderValues();

    Serial.println("Received slider values.");
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