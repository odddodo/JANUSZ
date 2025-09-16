#ifndef SESSION_H
#define SESSION_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class SessionManager {
    // Current active and waiting tokens
    String activeToken;
    unsigned long lastActive = 0;          // last activity/heartbeat from active

    String waitingToken;
    unsigned long countdownStart = 0;      // when the countdown started (only when someone is waiting)

    // When someone is waiting, how long the active user can keep control
    const unsigned long countdownDuration = 60000; // 60s countdown

    // If the active user disappears (no heartbeats/requests), switch after this grace
    const unsigned long heartbeatGraceMs = 2000;   // ~7s grace to detect disconnects

public:
    SessionManager();
    void registerEndpoints(AsyncWebServer& server);

    bool isValid(const String& token);
    void reset();

private:
    String generateToken();
    void expireIfNeeded();
    void promoteWaiting(unsigned long now);
};

#endif
