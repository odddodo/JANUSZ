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

// Expire logic: handles countdown and switching
void SessionManager::expireIfNeeded() {
    unsigned long now = millis();

    // If countdown expired → promote waiting user
    if (!waitingToken.isEmpty() && (now - countdownStart > countdownDuration)) {
        Serial.println("Countdown expired → switching active user");
        activeToken = waitingToken;
        lastActive = now;
        waitingToken = "";
        countdownStart = 0;
    }
}

void SessionManager::registerEndpoints(AsyncWebServer& server) {
    // --- SESSION REQUEST ---
    server.on("/session", HTTP_GET, [this](AsyncWebServerRequest* request) {
        expireIfNeeded();
        unsigned long now = millis();

        // No active session
        if (activeToken.isEmpty()) {
            activeToken = generateToken();
            lastActive = now;

            String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":0}";
            request->send(200, "application/json", body);

            Serial.println("New active session granted: " + activeToken);
            return;
        }

        // Already active, but check if this request is from the active user
        String token = request->header("X-Token");
        if (token == activeToken) {
            // Still active
            if (waitingToken.isEmpty()) {
                // No countdown if no one is waiting
                String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":0}";
                request->send(200, "application/json", body);
            } else {
                // Countdown is running
                unsigned long elapsed = now - countdownStart;
                unsigned long timeLeft = (elapsed < countdownDuration) ? (countdownDuration - elapsed) / 1000 : 0;

               String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":" + String(timeLeft) + "}";

                request->send(200, "application/json", body);
            }
            return;
        }

        // If already active but no waiting → make this client the waiting user
        if (waitingToken.isEmpty()) {
            waitingToken = generateToken();
            countdownStart = now;

           String body = String("{\"active\":false,\"token\":\"") + waitingToken + "\",\"timeLeft\":" + String(countdownDuration / 1000) + "}";

            request->send(200, "application/json", body);

            Serial.println("New waiting user queued: " + waitingToken);
            return;
        }

        // Already waiting → update countdown
        unsigned long elapsed = now - countdownStart;
        unsigned long timeLeft = (elapsed < countdownDuration) ? (countdownDuration - elapsed) / 1000 : 0;
       String body = String("{\"active\":false,\"token\":\"") + waitingToken + "\",\"timeLeft\":" + String(timeLeft) + "}";

        request->send(200, "application/json", body);
    });
}

// Validate token & refresh activity timer
bool SessionManager::isValid(const String& token) {
    expireIfNeeded();

    if (token == activeToken) {
        lastActive = millis();
        return true;
    }
    return false;
}

// Optional: force reset
void SessionManager::reset() {
    activeToken = "";
    waitingToken = "";
    countdownStart = 0;
}
