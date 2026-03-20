#include "MakapoWebServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <uri/UriBraces.h>

// Implementation of constructor
MakapoWebServer::MakapoWebServer(PaddlerDataBuffer& buffer)
    : server(80), dataBuffer(buffer)
{
}

void MakapoWebServer::begin() {
  server.on("/", HTTP_GET, [this]() {
    handleRoot();
  });

  server.on(UriBraces("{}/{}"), HTTP_GET, [this]() {
    int paddlerId = server.pathArg(0).toInt();

    if (!validId(paddlerId) || paddlerId <= 0) {
      handleNotFound("Invalid Paddler Id");
      return;
    }

    String dataRequested = server.pathArg(1);

    if (dataRequested == "paddlerData") {
      handlePaddlerData(paddlerId);
    }
    else if (dataRequested == "speed") {
      handleSpeed(paddlerId);
    }
    else if (dataRequested == "location") {
      handleLocation(paddlerId);
    }
    else if (dataRequested == "strokeRate") {
      handleStrokeRate(paddlerId);
    }
    else if (dataRequested == "avgStrokeForce") {
      handleAvgStrokeForce(paddlerId);
    }
    else {
      handleNotFound("This endpoint doesn't exist");
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

void MakapoWebServer::handleSpeed(int id) {
  StaticJsonDocument<128> doc;
  doc["paddlerId"] = id;
  doc["speed"] = 2.5;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleLocation(int id) {
  StaticJsonDocument<128> doc;
  doc["paddlerId"] = id;
  doc["location"] = "X,Y";

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleStrokeRate(int id) {
  StaticJsonDocument<128> doc;
  doc["paddlerId"] = id;
  doc["strokeRate"] = 30;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleAvgStrokeForce(int id) {
  StaticJsonDocument<128> doc;
  doc["paddlerId"] = id;
  doc["avgStrokeForce"] = 45;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handlePaddlerData(int id) {
  StaticJsonDocument<256> doc;

  doc["paddlerId"] = id;
  doc["speed"] = 2.5;
  doc["location"] = "X,Y";
  doc["strokeRate"] = 30;
  doc["avgStrokeForce"] = 45;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

bool MakapoWebServer::validId(int paddlerId) {
  return paddlerId > 0;
}

void MakapoWebServer::handleNotFound(String error) {
  StaticJsonDocument<128> doc;
  doc["error"] = error;

  String body;
  serializeJson(doc, body);
  server.send(404, "application/json", body);
}