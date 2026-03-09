#pragma once

#include <WebServer.h>
#include "PaddlerDataBuffer.h"

class MakapoWebServer {
public:
    explicit MakapoWebServer(PaddlerDataBuffer& buffer);

    void begin();   //starts HTTP server and creates soft WiFi access point
    void handleClient();   // call in loop() to listen for incoming HTTP reqs

private:
    WebServer server;                
    PaddlerDataBuffer& dataBuffer;     

    void handleRoot();
    void handleSpeed(int paddlerId);
    void handleLocation(int paddlerId);
    void handleStrokeRate(int paddlerId);
    void handleAvgStrokeForce(int paddlerId);
    void handlePaddlerData(int paddlerId);

    bool validId(int paddlerId);
    void handleNotFound(String string);
};