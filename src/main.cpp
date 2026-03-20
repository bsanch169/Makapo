#include <WiFi.h>
#include "MakapoWebServer.h"
#include "PaddlerDataBuffer.h"

const char* ssid = "Makapo";
const char* password = "123456789";

PaddlerDataBuffer buffer;
MakapoWebServer webServer(buffer);

void setup() {
  Serial.begin(115200);

  bool result = WiFi.softAP(ssid, password);

  if (result) {
    Serial.println("Soft AP started successfully");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Soft AP failed to start");
  }

  webServer.begin();
  Serial.println("Web server started");
}

void loop() {
  webServer.handleClient();
}