#ifndef TALKING_H
#define TALKING_H

#include <Wire.h>
#include "parameters.h"
#include <Arduino.h>
#include <website.h>

#define SAVE_COMMAND_BYTE 0xFF
#define ERROR_CODE        0xEE   // Special code for error signaling

uint8_t dataArray[SLIDERSCOUNT];
uint8_t lastSentArray[SLIDERSCOUNT];
uint8_t checksum;

#define TRANSMIT_INTERVAL_MS 100
unsigned long lastTransmitTime = 0;

String slidersToJSON() {
    DynamicJsonDocument doc(1024);
    JsonArray arr = doc.to<JsonArray>();

    for (int i = 0; i < SLIDERSCOUNT; i++) {
        arr.add(sliderValues[i]);
    }

    String json;
    serializeJson(doc, json);
    return json;   // ✅ fixed bug (was missing return)
}

uint8_t calculateChecksum(const uint8_t *data, size_t length) {
    uint8_t cs = 0;
    for (size_t i = 0; i < length; i++) {
        cs ^= data[i];
    }
    return cs;
}

// Request slider values from the slave with retries and error handling
bool requestSlidersFromSlave() {
    for (int attempt = 0; attempt < 3; attempt++) {
        Wire.requestFrom(I2C_SLAVE_ADDR, SLIDERSCOUNT + 1);

        if (Wire.available() < SLIDERSCOUNT + 1) {
            Serial.println("Not enough data received from slave");
            continue; // retry
        }

        uint8_t buffer[SLIDERSCOUNT];
        for (int i = 0; i < SLIDERSCOUNT; i++) {
            buffer[i] = Wire.read();
        }

        uint8_t receivedChecksum = Wire.read();

        // ✅ Detect explicit error code from slave
        if (receivedChecksum == ERROR_CODE) {
            Serial.println("Slave reported checksum error, retrying...");
            continue; // retry
        }

        // Verify checksum
        uint8_t calculatedChecksum = calculateChecksum(buffer, SLIDERSCOUNT);
        if (calculatedChecksum == receivedChecksum) {
            memcpy(sliderValues, buffer, SLIDERSCOUNT);
            memcpy(dataArray, buffer, SLIDERSCOUNT);
            memcpy(lastSentArray, buffer, SLIDERSCOUNT);
            Serial.println("Sliders updated from slave");
            return true;
        }

        Serial.println("Checksum mismatch from slave, retrying...");
    }
    Serial.println("Failed to update sliders after retries.");
    return false;
}

void init_talking() {
    Wire.begin(I2C_SDA, I2C_SCL); // Master mode
    requestSlidersFromSlave();
}

void initSerial() {
    Serial.begin(115200);
}

void update_values() {
    memcpy(dataArray, sliderValues, sizeof(sliderValues));
}

bool hasChanged() {
    for (int i = 0; i < SLIDERSCOUNT; i++) {
        if (dataArray[i] != lastSentArray[i]) return true;
    }
    return false;
}

void talkI2C(bool sendSave = false, bool sendLoad = false) {
    update_values();
    unsigned long now = millis();

    if (!hasChanged() && (now - lastSendTime < HEARTBEAT_INTERVAL))
        return; // Skip

    checksum = calculateChecksum(dataArray, SLIDERSCOUNT);

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(dataArray, SLIDERSCOUNT);
    Wire.write(checksum);

    if (sendSave) {
        Wire.write(SAVE_COMMAND_BYTE);
    }

    uint8_t err = Wire.endTransmission();

    if (err == 0) {
        //Serial.println("Data sent");
        memcpy(lastSentArray, dataArray, SLIDERSCOUNT);
        lastSendTime = now;
    } else {
        Serial.print("I2C error: ");
        Serial.println(err);
    }
}

void init_values() {
    for (int i = 0; i < SLIDERSCOUNT; i++) {
        dataArray[i] = 127;
    }
}

#endif
