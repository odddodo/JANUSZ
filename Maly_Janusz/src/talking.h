#ifndef TALKING_H
#define TALKING_H

#include <Wire.h>
#include "parameters.h"
#include "functions.h"


#define I2C_SLAVE_ADDR 0x08

#define SAVE_COMMAND_BYTE 0xFF
#define LOAD_COMMAND_BYTE 0xFE

bool dataReady = false;
bool saveTriggered = false;

// Initialize default values
void initSliderValues()
{
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
        // Handle save logic
        saveTriggered = true;
        Serial.println("saving triggered");
      }
    }
  } else {
    Serial.println("Checksum mismatch!");
  }
}

// I2C request handler (if master asks for data)
void requestEvent() {
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

    // Use Wire (or Wire1 / Wire2 depending on platform)
   
#//if defined(ARDUINO_ARCH_ESP32)
    Wire2.begin(I2C_SLAVE_ADDR);
    //Wire2.setClock(400000);
        Wire2.setSCL(24);  // Or your actual SCL pin
    Wire2.setSDA(25);  // Or your actual SDA pin
//#endif

    Wire2.onReceive(receiveEvent);
    Wire2.onRequest(requestEvent);

    Serial.println("Slave device initialized and ready.");
}

#endif
