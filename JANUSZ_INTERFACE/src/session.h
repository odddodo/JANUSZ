#ifndef SESSION_H
#define SESSION_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class SessionManager {
    String activeToken;
    unsigned long lastActive = 0;

    String waitingToken;
    unsigned long countdownStart = 0;
    const unsigned long countdownDuration = 60000; // 60s countdown when a waiting user exists

public:
    SessionManager();
    void registerEndpoints(AsyncWebServer& server);

    bool isValid(const String& token);
    void reset();

private:
    String generateToken();
    void expireIfNeeded();
};

#endif
