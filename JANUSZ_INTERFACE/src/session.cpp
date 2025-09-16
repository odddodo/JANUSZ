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

void SessionManager::promoteWaiting(unsigned long now) {
    if (!waitingToken.isEmpty()) {
        Serial.println("Promoting waiting user to active");
        activeToken = waitingToken;
        lastActive = now;
        waitingToken = "";
        countdownStart = 0;
    } else {
        // No one waiting → simply clear active
        activeToken = "";
        countdownStart = 0;
    }
}

void SessionManager::expireIfNeeded() {
    unsigned long now = millis();

    // If someone is waiting, two things can end the current active session:
    // 1) Countdown expires
    // 2) Active user stops heartbeating/being active for > heartbeatGraceMs
    if (!waitingToken.isEmpty()) {
        // If active vanished (no recent activity/heartbeat), switch early
        if (!activeToken.isEmpty() && (now - lastActive > heartbeatGraceMs)) {
            Serial.println("No heartbeat from active user → early switch");
            promoteWaiting(now);
            return;
        }

        // Normal countdown expiry
        if (countdownStart && (now - countdownStart > countdownDuration)) {
            Serial.println("Countdown expired → switching");
            promoteWaiting(now);
            return;
        }
    }
}

void SessionManager::registerEndpoints(AsyncWebServer& server) {
    // --- SESSION REQUEST ---
    server.on("/session", HTTP_GET, [this](AsyncWebServerRequest* request) {
        expireIfNeeded();
        unsigned long now = millis();

        // If no active session, grant immediately
        if (activeToken.isEmpty()) {
            activeToken = generateToken();
            lastActive = now;

            String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":0}";
            request->send(200, "application/json", body);

            Serial.println("New active session granted: " + activeToken);
            return;
        }

        // Identify caller (may be active or waiting)
        String callerToken = request->header("X-Token");

        // Active asking for status
        if (callerToken == activeToken) {
            lastActive = now; // touch
            if (waitingToken.isEmpty()) {
                // No one waiting → no countdown
                String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":0}";
                request->send(200, "application/json", body);
            } else {
                // Countdown running
                unsigned long elapsed = now - countdownStart;
                unsigned long timeLeft = (elapsed < countdownDuration) ? (countdownDuration - elapsed) / 1000 : 0;
                String body = String("{\"active\":true,\"token\":\"") + activeToken + "\",\"timeLeft\":" + String(timeLeft) + "}";
                request->send(200, "application/json", body);
            }
            return;
        }

        // Not active
        if (waitingToken.isEmpty()) {
            // First waiter arrives → start countdown
            waitingToken = generateToken();
            countdownStart = now;

            String body = String("{\"active\":false,\"token\":\"") + waitingToken + "\",\"timeLeft\":" + String(countdownDuration / 1000) + "}";
            request->send(200, "application/json", body);

            Serial.println("Waiting user queued: " + waitingToken);
            return;
        }

        // Already have a waiting user → report remaining time to anyone else
        unsigned long elapsed = now - countdownStart;
        unsigned long timeLeft = (elapsed < countdownDuration) ? (countdownDuration - elapsed) / 1000 : 0;
        String body = String("{\"active\":false,\"token\":\"") + waitingToken + "\",\"timeLeft\":" + String(timeLeft) + "}";
        request->send(200, "application/json", body);
    });

    // --- HEARTBEAT ENDPOINT (active user pings every few seconds) ---
    server.on("/heartbeat", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String t = request->header("X-Token");
        if (t == activeToken) {
            lastActive = millis(); // refresh last active time
            request->send(200, "text/plain", "OK");
        } else {
            request->send(403, "text/plain", "Invalid or not active");
        }
    });

    // --- RELEASE ENDPOINT (called on page/tab close via sendBeacon) ---
    // Uses a POST with query param ?t=<token> (sendBeacon can't set headers)
    server.on("/release", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String t = "";
        if (request->hasParam("t", true)) {
            t = request->getParam("t", true)->value();
        }
        // Also accept header if present
        if (t.isEmpty()) {
            t = request->header("X-Token");
        }

        if (!t.isEmpty() && t == activeToken) {
            unsigned long now = millis();
            Serial.println("Active user released session");
            promoteWaiting(now); // instantly hand over if someone is waiting
            request->send(200, "text/plain", "released");
        } else {
            request->send(403, "text/plain", "not active or bad token");
        }
    });
}

// Validate token & refresh activity timer (used by write endpoints)
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
    lastActive = 0;
}
