#include "MakapoWebServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <uri/UriBraces.h>

MakapoWebServer::MakapoWebServer(PaddlerDataBuffer& buffer)
    : server(80), dataBuffer(buffer) {
}

void MakapoWebServer::begin() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });

  // {boatId}/{boatData}
  server.on(UriBraces("{}/{}"), HTTP_GET, [this]() {
    uint8_t boatID = server.pathArg(0).toInt();
    String dataRequested = server.pathArg(1);

    if (!dataBuffer.hasData(boatID)) {
      handleNotFound("Invalid Boat ID, or no data from this boat yet");
      return;
    } 

    if (dataRequested == "boatData") {
      handleBoatData(boatID);
    }
    else {
      handleNotFound("This endpoint doesn't exist");
    }
  });

  // /{boatId}//{paddlerId}/{paddlerData}
  server.on(UriBraces("{}/{}/{}"), HTTP_GET, [this]() {
    uint8_t boatID = server.pathArg(0).toInt();
    String dataRequested = server.pathArg(1);
    uint8_t paddlerID = server.pathArg(2).toInt();

    if (!dataBuffer.hasData(boatID)) {
      handleNotFound("Invalid Boat ID, or no data from this boat yet");
      return;
    }

    if (dataRequested == "paddlerData") {
      handlePaddlerData(boatID, paddlerID);
    }
    else {
      handleNotFound("The endpoint " + dataRequested + " doesn't exist");
    }
  });

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