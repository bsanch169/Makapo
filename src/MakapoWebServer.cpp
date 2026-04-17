#include "MakapoWebServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <uri/UriBraces.h>

const char* ssid = "Makapo";
const char* password = "123456789";

void handleClientTask(void* pvParameters) {
    MakapoSharedParams* params = (MakapoSharedParams*)pvParameters;

    MakapoWebServer webServer(*params->liveStorage);
    webServer.begin();

    while (true) {
        webServer.handleClient();
        vTaskDelay(1);
    }
}

MakapoWebServer::MakapoWebServer(PaddlerDataBuffer& buffer)
    : server(80), dataBuffer(buffer) {
}

void MakapoWebServer::begin() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });

  // {boatId}/
  server.on(UriBraces("/{}"), HTTP_GET, [this]() {
    Serial.println("Boat endpoint hit");
    uint8_t boatID = server.pathArg(0).toInt();

    if (!dataBuffer.hasData(boatID)) {
      handleNotFound("Invalid Boat ID, or no data from this boat yet");
      return;
    } 

    handleBoatData(boatID);
  });

  // /{boatId}//{paddlerId}
  server.on(UriBraces("/{}/{}"), HTTP_GET, [this]() {
    uint8_t boatID = server.pathArg(0).toInt();
    uint8_t paddlerID = server.pathArg(1).toInt();

    if (!dataBuffer.hasData(boatID)) {
      handleNotFound("Invalid Boat ID, or no data from this boat yet");
      return;
    }

    handlePaddlerData(boatID, paddlerID);
  });

  bool result = WiFi.softAP(ssid, password);

  if (result) {
    Serial.println("Soft AP started successfully");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Soft AP failed to start");
  }

  server.begin();
}

void MakapoWebServer::handleClient() {
  server.handleClient();
}

void MakapoWebServer::handleRoot() {
  server.send(200, "text/plain", "REST API for Makapo");
}

void MakapoWebServer::handleBoatData(uint8_t boatId) {
  BoatData data;
  
  if (!(dataBuffer.getLatestBoatData(boatId, data))) {
    handleNotFound("No data found for this boat");
    return;
  }

  StaticJsonDocument<2048> doc;
  doc["boatID"] = data.boatID;
  doc["paddlerCount"] = data.paddlerCount;
  doc["senStatus"] = data.senStatus;
  doc["boatStatus"] = data.boatStatus;

  doc["speed"] = data.speed;
  doc["lat"] = data.coordLat;
  doc["long"] = data.coordLon;
  doc["videoID"] = data.videoID;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
  JsonArray paddlers = doc.createNestedArray("paddlers");
  for (size_t i = 0; i < data.paddlerCount; i++) {
    JsonObject p = paddlers.createNestedObject();
    p["paddlerId"] = data.paddlers[i].paddlerID;
    p["paddleAng"] = data.paddlers[i].paddleAng;
    p["paddleVel"] = data.paddlers[i].paddleVel;
    p["paddlePres"] = data.paddlers[i].paddlePres;
    p["strokeRate"] = data.paddlers[i].strokeRate;
  }

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handlePaddlerData(uint8_t boatID, uint8_t paddlerID) {
  BoatData data; 

  if (!dataBuffer.getLatestBoatData(boatID, data)) {
    handleNotFound("No data found for this boat");
    return;
  }

  bool found = false;
  PaddlerData paddler;

  for (size_t i = 0; i < data.paddlerCount; i++) {
    if (data.paddlers[i].paddlerID == paddlerID) {
      paddler = data.paddlers[i];
      found = true;
      break;
    }
  }

  if (!found) {
    handleNotFound("Paddler not found for this boat");
    return;
  }

  StaticJsonDocument<256> doc;
  doc["boatID"] = boatID;
  doc["paddlerID"] = paddlerID;
  doc["paddleAng"] = paddler.paddleAng;
  doc["paddleVel"] = paddler.paddleVel;
  doc["paddlePres"] = paddler.paddlePres;
  doc["strokeRate"] = paddler.strokeRate;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleNotFound(String error) {
  StaticJsonDocument<128> doc;
  doc["error"] = error;

  String body;
  serializeJson(doc, body);
  server.send(404, "application/json", body);
}