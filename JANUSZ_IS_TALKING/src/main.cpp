#include <WiFi.h>
#include <WebServer.h>
#include <webpage.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

const char* ssid = "JANUSZ_INTERFACE";
const char* password = "";  // Open network

#define I2C_SLAVE_ADDR 0x08 // I2C address of the slave ESP32
#define SAVE_COMMAND_BYTE 0xFF 
#define I2C_TRANSFER_BYTES 4

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

volatile bool  sendColorFlag=false;

// Store latest color
int currentR = 255, currentG = 0, currentB = 0, currentS = 0;


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendColorToSlave(uint8_t r, uint8_t g, uint8_t b, uint8_t s) {
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.write(s);
  Wire.endTransmission();
  sendColorFlag=false;
}

void setup() {
  Serial.begin(9600);
Wire.begin();
  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // Serve the main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlPage);
  });

  // Receive color values from JS
  server.on("/setcolor", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("r")) currentR = request->getParam("r")->value().toInt();
    if (request->hasParam("g")) currentG = request->getParam("g")->value().toInt();
    if (request->hasParam("b")) currentB = request->getParam("b")->value().toInt();
    if (request->hasParam("s")) currentB = request->getParam("s")->value().toInt();

    Serial.printf("Color received: R=%d G=%d B=%d\n", currentR, currentG, currentB);
    sendColorFlag = true;
     
    request->send(200, "text/plain", "OK");
  });

  server.onNotFound(notFound);
  server.begin();
}
void loop(){

    if (sendColorFlag) {
    sendColorToSlave(currentR, currentG, currentB, currentS);
    sendColorFlag = false;
  }
}

