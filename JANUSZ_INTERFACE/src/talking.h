#ifndef TALKING_H
#define TALKING_H

#include <Wire.h>
#include <parameters.h>
#include <Arduino.h>

uint8_t dataArray[SLIDERSCOUNT]; // Your 42 values
uint8_t checksum;

#define TRANSMIT_INTERVAL_MS 10 // How often to send data
unsigned long lastTransmitTime = 0;

uint8_t calculateChecksum(const uint8_t *data, size_t length)
{
    uint8_t cs = 0;
    for (size_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }
    return cs;
}

void init_talking()
{
    Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ); // Master mode, set freq
}
void initSerial()
{
    Serial.begin(115200);

}

void talkI2C()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastTransmitTime >= TRANSMIT_INTERVAL_MS)
    {
        lastTransmitTime = currentMillis;

        checksum = calculateChecksum(dataArray, 42);

        Wire.beginTransmission(I2C_SLAVE_ADDR);
        Wire.write(dataArray, 42);
        Wire.write(checksum);
        uint8_t err = Wire.endTransmission();

        if (err == 0)
        {
            Serial.println("Data sent successfully");
        }
        else
        {
            Serial.print("I2C Transmission error: ");
            Serial.println(err);
        }

        // (Optional) update dataArray here if needed
    }
}

#endif