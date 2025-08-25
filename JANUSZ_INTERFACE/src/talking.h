#ifndef TALKING_H
#define TALKING_H

#include <Wire.h>
#include <parameters.h>
#include <Arduino.h>
#include <website.h>

uint8_t dataArray[SLIDERSCOUNT]; // Your 42 values
uint8_t lastSentArray[SLIDERSCOUNT];
uint8_t checksum;

#define TRANSMIT_INTERVAL_MS 100 // How often to send data
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
    Wire.begin(I2C_SDA, I2C_SCL); // Master mode, set freq
}
void initSerial()
{
    Serial.begin(115200);
}
void update_values(){
memcpy(dataArray, sliderValues, sizeof(sliderValues));
}
bool hasChanged()
{
    for (int i = 0; i < SLIDERSCOUNT; i++)
    {
        if (dataArray[i] != lastSentArray[i]) return true;
    }
    return false;
}
void talkI2C()
{
    update_values();
    unsigned long now = millis();

    if (!hasChanged() && (now - lastSendTime < HEARTBEAT_INTERVAL))
        return; // Skip

    checksum = calculateChecksum(dataArray, SLIDERSCOUNT);

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(dataArray, SLIDERSCOUNT);
    Wire.write(checksum);
    uint8_t err = Wire.endTransmission();

    if (err == 0)
    {
        Serial.println("Data sent");
        memcpy(lastSentArray, dataArray, SLIDERSCOUNT);
        lastSendTime = now;
    }
    else
    {
        Serial.print("I2C error: ");
        Serial.println(err);
    }
}

void init_values(){
  for(int i=0;i<SLIDERSCOUNT;i++){
    dataArray[i]=127;
  } 
}



#endif