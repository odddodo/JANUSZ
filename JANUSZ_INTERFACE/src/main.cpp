#include <Arduino.h>
#include <WiFi.h>
#include <website.h>

const char* ssid = "ESP32_RADIAL";
const char* password = "12345678";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Start WiFi in AP mode
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  // Nothing needed here for now
}
