#ifndef PARAMETERS_H
#define PARAMETERS_H
#include <Arduino.h>
#include <ArduinoJson.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 400000   // 400 kHz Fast Mode
#define I2C_SLAVE_ADDR 0x08 // Change to your slave address

#define SLIDERSCOUNT 30

const char* ssid = "JANUSZ_INTERFACE";
const char* password = "Hello_Janusz";

uint8_t sliderValues[SLIDERSCOUNT] = {0};
int debugFrequency=1000;

#define HEARTBEAT_INTERVAL 1000
unsigned long lastSendTime = 0;

#endif