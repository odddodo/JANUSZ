#ifndef MEMORY_H
#define MEMORY_H
#include <SD.h>
#include <SPI.h>

#include <parameters.h>

const int chipSelect = BUILTIN_SDCARD;  // Teensy 4.1 SD slot

const char* filename = "remember.bin";
//const int arraySize = 100;

//float myArray[arraySize];  // Example array of floats

// ========================
// Save Array to SD
// ========================
void initMemory(){
     // while (!Serial);  // Wait for Serial if needed

  if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
    return;
  }

  Serial.println("SD card ready.");
}

bool saveArrayToSD(uint8_t* data, int len) {
  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing.");
    return false;
  }

  size_t written = file.write(data, len);
  file.close();

  if (written != len) {
    Serial.println("Write failed or incomplete.");
    return false;
  }

  Serial.println("Array saved successfully.");
  return true;
}

// ========================
// Load Array from SD
// ========================
bool loadArrayFromSD(uint8_t* data, int len) {
  if (!SD.exists(filename)) {
    Serial.println("File doesn't exist.");
    return false;
  }

  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return false;
  }

  size_t read = file.read(data, len);
  file.close();

  if (read != len) {
    Serial.println("Read incomplete.");
    return false;
  }

  Serial.println("Array loaded successfully.");
  return true;
}


#endif