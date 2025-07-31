#include <Arduino.h>
#include <WiFi.h>
#include <website.h>
#include <talking.h>
#include <parameters.h>

AsyncWebServer server(80);

int sliderValues[SLIDERSCOUNT] = {0};
unsigned long lastDebugTime = 0;

void debug()
{
  unsigned long now = millis();
  if (now - lastDebugTime >= debugFrequency)
  {
    Serial.println();
    for (int i = 0; i < 20; i++)
    {
      Serial.printf("%02d:%d", i, sliderValues[i]);
      if (i < 41)
        Serial.print(", ");
    }
    Serial.println();
    lastDebugTime = now;
  }
}

void setup()
{
 
initSerial();
  init_talking();

  // Start WiFi in AP mode
  WiFi.softAP(ssid, password);
    Serial.println("Access Point started");
    Serial.println(WiFi.softAPIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });
  // Endpoint to receive slider updates
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("i") && request->hasParam("v")) {
      int index = request->getParam("i")->value().toInt();
      int value = request->getParam("v")->value().toInt();
      if (index >= 0 && index < SLIDERSCOUNT) {
        sliderValues[index] = value;
      }
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    } });

  server.begin();
}

void loop()
{
  debug();
  talkI2C();
}
