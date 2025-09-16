#include <Arduino.h>
#include <WiFi.h>
#include <talking.h>
#include <parameters.h>
#include <session.h>
#include <website.h>

AsyncWebServer server(80);
SessionManager sessions;

void setup() {
  initSerial();
  init_values();
  init_talking();

  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  // Serve HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Token issuing
  sessions.registerEndpoints(server);

  // Slider updates (write to slave)
  server.on("/sliders", HTTP_POST,
    [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String token = request->header("X-Token");
      if (!sessions.isValid(token)) {
        request->send(403, "text/plain", "Invalid or expired token");
        return;
      }

      String body;
      for (size_t i = 0; i < len; i++) body += (char)data[i];

      DynamicJsonDocument doc(1024);
      if (deserializeJson(doc, body)) {
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
      }

      talkI2C();  // ✅ Only run if valid token
      request->send(200, "text/plain", "OK");
    }
  );

  // Get current sliders (read from slave)
  server.on("/get_sliders", HTTP_GET, [](AsyncWebServerRequest *request) {
    String token = request->header("X-Token");
    if (!sessions.isValid(token)) {
      request->send(403, "application/json", "{\"error\":\"Invalid or expired token\"}");
      return;
    }

    if (!requestSlidersFromSlave()) {
      request->send(500, "application/json", "{\"error\":\"Failed to read from slave\"}");
      return;
    }

    String json = slidersToJSON();
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  talkI2C();
}
