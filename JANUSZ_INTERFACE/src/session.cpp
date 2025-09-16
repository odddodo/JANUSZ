#include "session.h"
#include <esp_system.h>   // for esp_random()

SessionManager::SessionManager() {
    randomSeed(esp_random());
}

// Generate a random token
String SessionManager::generateToken() {
    char buf[25];
    uint32_t r1 = esp_random();
    uint32_t r2 = esp_random();
    snprintf(buf, sizeof(buf), "%08X%08X%08X", r1, r2, (uint32_t)millis());
    return String(buf);
}

void SessionManager::registerEndpoints(AsyncWebServer& server) {
    server.on("/session", HTTP_GET, [this](AsyncWebServerRequest* request) {
        unsigned long now = millis();

        // Expire old session if needed
        if (activeToken.length() && (now - lastActive > timeoutMs)) {
            Serial.println("Session expired, clearing token");
            activeToken = "";
        }

        if (activeToken.isEmpty()) {
            // Grant a new token
            activeToken = generateToken();
            lastActive = now;

            unsigned long remaining = timeoutMs / 1000;
            String body = String("{\"token\":\"") + activeToken + "\",\"remaining\":" + remaining + "}";
            request->send(200, "application/json", body);

            Serial.println("New session granted: " + activeToken);
        } else {
            // Already taken → tell client how long to wait
            unsigned long elapsed = now - lastActive;
            unsigned long timeLeft = (elapsed < timeoutMs) ? (timeoutMs - elapsed) / 1000 : 0;

            String body = String("{\"status\":\"busy\",\"retryAfter\":") + timeLeft + "}";
            request->send(200, "application/json", body);

            Serial.println("Session busy, retry after " + String(timeLeft) + "s");
        }
    });
}

// Validate token & refresh activity timer
bool SessionManager::isValid(const String& token) {
    unsigned long now = millis();

    // Expire old session
    if (activeToken.length() && (now - lastActive > timeoutMs)) {
        Serial.println("Session expired, clearing token");
        activeToken = "";
        return false;
    }

    if (token == activeToken) {
        lastActive = now;
        return true;
    }
    return false;
}

// Optional: force reset
void SessionManager::reset() {
    activeToken = "";
}
