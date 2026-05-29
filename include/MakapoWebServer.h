#pragma once
#include <WebServer.h>
#include "PaddlerDataBuffer.h"
#include "RawPacket.h"


void handleClientTask(void* pvParameters);

class MakapoWebServer {
public:
    explicit MakapoWebServer(PaddlerDataBuffer& buffer, SemaphoreHandle_t mutex, volatile bool& radioMode);
    void begin();   //starts HTTP server and creates soft WiFi access point
    void handleClient();

private:
    WebServer server;                
    PaddlerDataBuffer& dataBuffer;  
    SemaphoreHandle_t mutex;
    volatile bool& radioMode;

    void handleRoot();
    void handleBoatData(uint8_t boatID);
    void handlePaddlerData(uint8_t boatID, uint8_t paddlerID);
    void handleMessage(uint8_t boatID, uint8_t messageID, String message = "");
    void handleNotFound(String string);
};