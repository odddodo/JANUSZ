#include <Arduino.h>

#include <WiFi.h>
#include <talking.h>
#include <parameters.h>



AsyncWebServer server(80);


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
  init_values();
  init_talking();
 

  // Start WiFi in AP mode
  WiFi.softAP(ssid, password);
    Serial.println("Access Point started");
    Serial.println(WiFi.softAPIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });
  // Endpoint to receive slider updates
server.on("/sliders", HTTP_POST, [](AsyncWebServerRequest *request){},
  NULL, // no upload handler
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // Parse JSON body
    String body;
    for (size_t i = 0; i < len; i++) body += (char)data[i];

    DynamicJsonDocument doc(1024); // enough for 42 numbers
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      request->send(400, "text/plain", "Invalid JSON");
      return;
    }

    JsonArray arr = doc.as<JsonArray>();
    if (arr.size() != SLIDERSCOUNT) {
      request->send(400, "text/plain", "Wrong number of sliders");
      return;
    }

    for (int i = 0; i < SLIDERSCOUNT; i++) {
      sliderValues[i] = arr[i].as<int>();
      // optionally also copy to dataArray if needed:
      // dataArray[i] = sliderValues[i];
    }

    request->send(200, "text/plain", "OK");
  }
);
server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    talkI2C(true, false); // send sliders + checksum + SAVE_COMMAND_BYTE
    request->send(200, "text/plain", "Save triggered");
});
server.on("/get_sliders", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!requestSlidersFromSlave()) {
        request->send(500, "application/json", "{\"error\":\"Failed to read from slave\"}");
        return;
    }

    String json = slidersToJSON();
    request->send(200, "application/json", json);
});
  server.begin();
}

void loop()
{
  //debug();
  talkI2C();
  
}
