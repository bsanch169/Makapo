#pragma once

#include <WebServer.h>
#include "PaddlerDataBuffer.h"

class MakapoWebServer {
public:
    explicit MakapoWebServer(PaddlerDataBuffer& buffer);

    void begin();
    void handleClient();

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
    void handleNotFound(String error = "This endpoint doesn't exist");
};