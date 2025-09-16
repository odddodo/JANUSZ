#ifndef SESSION_H
#define SESSION_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class SessionManager
{
    String activeToken;

    unsigned long lastActive = 0;
    const unsigned long timeoutMs = 60000; // 1 s inactivity timeout

public:
    SessionManager();
    void registerEndpoints(AsyncWebServer &server);

    bool isValid(const String &token);
    void reset();

private:
    String generateToken();
};

#endif
