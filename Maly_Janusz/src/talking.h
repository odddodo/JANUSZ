#ifndef talking_h
#define talking_h
#include <parameters.h>
#include <Wire.h>
#include <functions.h>

#define RECEIVE_BYTES 4
u_int8_t receivedValues[4];

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

void receiveEvent(int numBytes){
    int i = 0;

    // Read up to NUM_SLIDERS bytes into sliderValues
    while (Wire2.available() && i < RECEIVE_BYTES)
    {
        receivedValues[i++] = Wire2.read();
    }

    channels[0].mask = mapRange(receivedValues[0], 0, 255, 255, 0);
    channels[1].sinscl = mapRange(receivedValues[1], 0, 255, 0.01f, 0.1f);
    channels[2].sinscl= mapRange(receivedValues[2], 0, 255, 0.01f, 0.1f);
    channels[3].sinscl = mapRange(receivedValues[3], 0, 255, 0.01f, 0.1f);
    Serial.println("limited update!");
}
/*
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
    */
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