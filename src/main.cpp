#include <Arduino.h>
#include <WiFi.h>
#include "MakapoWebServer.h"
#include "PaddlerDataBuffer.h"

PaddlerDataBuffer buffer;
MakapoWebServer webServer(buffer);

void setup() {
  Serial.begin(115200);
  
  webServer.begin();
  Serial.println("Web server started");
  BoatData data = {};
  data.boatID = 0;
  data.paddlerCount = 1;
  buffer.addBoatData(data);
}

void loop() {
  webServer.handleClient();
}
