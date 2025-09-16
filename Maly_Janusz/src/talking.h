#ifndef TALKING_H
#define TALKING_H

#include <Wire.h>
#include "parameters.h"
#include "functions.h"

#define I2C_SLAVE_ADDR 0x08
#define SAVE_COMMAND_BYTE 0xFF
#define ERROR_CODE        0xEE   // Special error code

bool dataReady = false;
bool saveTriggered = false;
uint8_t lastErrorCode = 0;   // Tracks last error for master notification

// Initialize default values
void initSliderValues() {
    for (int i = 0; i < NUMSLIDERS; i++) {
        sliderValues[i] = 128;
    }
    updateSliderValues();
}

void receiveEvent(int numBytes) {
    if (numBytes < NUMSLIDERS + 1) return;

    uint8_t buffer[NUMSLIDERS];
    for (int i = 0; i < NUMSLIDERS; i++) {
        if (Wire2.available()) {
            buffer[i] = Wire2.read();
        }
    }

    if (!Wire2.available()) return;

    uint8_t receivedChecksum = Wire2.read();
    uint8_t calculatedChecksum = 0;
    for (int i = 0; i < NUMSLIDERS; i++) {
        calculatedChecksum ^= buffer[i];
    }

    if ((calculatedChecksum & 0xFF) == receivedChecksum) {
        memcpy(sliderValues, buffer, NUMSLIDERS);
        dataReady = true;

        if (Wire2.available()) {
            uint8_t command = Wire2.read();
            if (command == SAVE_COMMAND_BYTE) {
                saveTriggered = true;
                Serial.println("Saving triggered");
            }
        }
        lastErrorCode = 0;  // ✅ clear error on success
    } else {
        Serial.println("Checksum mismatch!");
        lastErrorCode = ERROR_CODE;  // ✅ flag error for next request
    }
}

// Master requests data → send slider values + checksum OR error
void requestEvent() {
    if (lastErrorCode != 0) {
        Wire2.write(lastErrorCode);  // ✅ notify master
        lastErrorCode = 0;           // reset error code after reporting
        return;
    }

    uint8_t checksum = 0;
    for (int i = 0; i < NUMSLIDERS; i++) {
        Wire2.write(sliderValues[i]);
        checksum ^= sliderValues[i];
    }
    Wire2.write(checksum);
}

// Setup function for the I2C slave
void initTalking() {
    Serial.begin(115200);

    // ESP32-specific Wire2 usage
    Wire2.begin(I2C_SLAVE_ADDR);
    Wire2.setSCL(24);
    Wire2.setSDA(25);

    Wire2.onReceive(receiveEvent);
    Wire2.onRequest(requestEvent);

    Serial.println("Slave device initialized and ready.");
}

#endif
