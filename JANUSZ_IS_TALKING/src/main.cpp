// Include necessary libraries
#include <WiFi.h>                  // Provides WiFi functionality
#include <Wire.h>                  // For I2C communication
#include <debug.h>
#include <ESPAsyncWebServer.h>    // Asynchronous web server for non-blocking HTTP and WebSocket handling
#include <webpage.h>              // Contains the HTML page content (defined elsewhere)

// Define constants
#define I2C_SLAVE_ADDR 0x08       // I2C address of the slave device (e.g., an Arduino or sensor)
#define SAVE_COMMAND_BYTE 0xFF    // Byte used to instruct slave to persist settings (e.g., to EEPROM)


unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 100; // ms (adjust as needed)

// Create WebServer and WebSocket on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");         // WebSocket endpoint at "/ws"

// Buffer to store slider values (NUMSLIDERS must be defined elsewhere)
int sliderValues[NUMSLIDERS] = {0};

// WiFi credentials for Access Point mode
const char *ssid = "JANUSZ_testing";
const char *password = "12345678";

// Function to request slider values from the I2C slave and send to WebSocket client
void sendSliderValuesToClient(AsyncWebSocketClient *client) {
  Wire.requestFrom(I2C_SLAVE_ADDR, NUMSLIDERS + 1);  // Request bytes: sliders + checksum

  uint8_t buffer[NUMSLIDERS];       // Temporary buffer for slider values
  uint8_t receivedChecksum = 0;     // Checksum byte from slave
  uint8_t computedChecksum = 0;     // Calculated checksum

  unsigned long start = millis();
  int i = 0;

  // Read all slider bytes within 50ms timeout
  while (i < NUMSLIDERS && (millis() - start < 50)) {
    if (Wire.available()) {
      buffer[i] = Wire.read();          // Read byte from I2C
      computedChecksum ^= buffer[i];    // Update checksum
      i++;
    } else {
      delay(1);                         // Brief pause to allow more data to arrive
    }
  }

  // Read checksum byte
  if (Wire.available()) {
    receivedChecksum = Wire.read();
  } else {
    Serial.println("Timeout waiting for checksum byte");
    return; // Exit if checksum not received
  }

  // Compare checksums to ensure data integrity
  if ((computedChecksum & 0xFF) == receivedChecksum) {
    for (int j = 0; j < NUMSLIDERS; j++) {
      sliderValues[j] = buffer[j];  // Store validated values
    }

    delay(1);  // Short pause to ensure I2C transmission finishes

    // Create comma-separated string of slider values
    String message = "";
    for (int j = 0; j < NUMSLIDERS; j++) {
      message += String(sliderValues[j]);
      if (j < NUMSLIDERS - 1) message += ",";
    }

    // Send to the connected WebSocket client
    if (client) {
      debugPrintSliderValues((uint8_t*)sliderValues, NUMSLIDERS, "Received from I2C");
      client->text(message);
    }
  } else {
    Serial.println("Checksum mismatch on I2C read"); // Data corrupted
  }
}

// Parses a comma-separated string of slider values and sends over I2C
// Optionally appends SAVE_COMMAND_BYTE to persist values
void parseAndSendValues(String message, bool triggerSave = false) {
  static uint8_t lastSent[NUMSLIDERS] = {0}; // Remember last values
  int startIndex = 0, endIndex = 0, valueIndex = 0;
  uint8_t parsed[NUMSLIDERS] = {0};

  // Parse CSV
  while (valueIndex < NUMSLIDERS && startIndex < message.length()) {
    endIndex = message.indexOf(',', startIndex);
    String token = (endIndex == -1) ? message.substring(startIndex)
                                    : message.substring(startIndex, endIndex);
    startIndex = (endIndex == -1) ? message.length() : endIndex + 1;

    token.trim();
    if (token.length() == 0) continue;

    int val = token.toInt();
    if (val < 0 || val > 255) val = 128;

    parsed[valueIndex++] = val;
  }

  // Check if values changed OR save was requested
  bool valuesChanged = triggerSave;
  for (int i = 0; i < NUMSLIDERS; i++) {
    if (parsed[i] != lastSent[i]) {
      valuesChanged = true;
      break;
    }
  }

  // Throttle updates to every updateInterval milliseconds
  if (!valuesChanged || millis() - lastUpdateTime < updateInterval) {
    return; // Skip sending if unchanged or too soon
  }

  lastUpdateTime = millis(); // Update timestamp

  // Send via I2C
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  uint8_t checksum = 0;

  for (int i = 0; i < NUMSLIDERS; i++) {
    Wire.write(parsed[i]);
    checksum ^= parsed[i];
    sliderValues[i] = parsed[i];
    lastSent[i] = parsed[i]; // Update last sent values
  }

  Wire.write(checksum & 0xFF);

  if (triggerSave) {
    Wire.write(SAVE_COMMAND_BYTE);
  }

  Wire.endTransmission();
  delay(1);

  // Broadcast
  String outMsg = "";
  for (int j = 0; j < NUMSLIDERS; j++) {
    outMsg += String(sliderValues[j]);
    if (j < NUMSLIDERS - 1) outMsg += ",";
  }

  ws.textAll(outMsg);
  debugPrintSliderValues((uint8_t*)sliderValues, NUMSLIDERS, "Broadcast to Clients");
}


// WebSocket event handler
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client %u connected\n", client->id());

    // Spawn a task to send slider values after short delay
    xTaskCreate(
        [](void *param) {
          AsyncWebSocketClient *c = (AsyncWebSocketClient *)param;
          delay(1000);                  // Give frontend time to load
          sendSliderValuesToClient(c);  // Send initial slider states
          vTaskDelete(NULL);            // Clean up task
        },
        "InitSliderSender",
        4096,        // Stack size
        (void *)client,
        1,           // Priority
        NULL
    );

  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // If full frame and text
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String message = String((char *)data);

      // If message starts with "SAVE:", trigger save
      if (message.startsWith("SAVE:")) {
        message.remove(0, 5); // Remove prefix
        parseAndSendValues(message, true);
      } else {
        parseAndSendValues(message); // Just update values
      }
    }
  }
}

// Initialization function
void setup() {
  Serial.begin(9600);
  delay(1000);  // Allow serial monitor to connect

  // Start WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP()); // Print local IP address

  Wire.begin();  // Initialize I2C as master

  // Register WebSocket event handler
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Handle HTTP GET request for root page ("/")
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlPage); // Serve HTML page
  });

  server.begin(); // Start HTTP and WebSocket server
}

// Main loop – unused in this program due to async event-driven model
void loop() {
  // Nothing here
}
