#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

void debugPrintSliderValues(const uint8_t *values, int count, const String &label = "Slider Values") {
  bool allZero = true;
  for (int i = 0; i < count; i++) {
    if (values[i] != 0) {
      allZero = false;
      break;
    }
  }

  if (allZero) return;  // Skip printing if everything is zero

  Serial.print(label + ": ");
  for (int i = 0; i < count; i++) {
    Serial.print(values[i]);
    if (i < count - 1) Serial.print(", ");
  }
  Serial.println();
}



#endif