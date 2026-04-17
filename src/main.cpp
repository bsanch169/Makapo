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
  BoatData data0 = {};
  data0.boatID = 0;
  data0.paddlerCount = 1;
  buffer.addBoatData(data0);

  BoatData data1 = {};
  data1.boatID = 1;
  data1.paddlerCount = 3;
  buffer.addBoatData(data1);

  BoatData data2 = {};
  data2.boatID = 4;
  data2.paddlerCount = 6;
  buffer.addBoatData(data2);
  
  Serial.println(buffer.toString());
}

void loop() {
  webServer.handleClient();
}
