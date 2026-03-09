#include "MakapoWebServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <uri/UriBraces.h>

//Implementation of constructor
MakapoWebServer::MakapoWebServer(PaddlerDataBuffer& buffer)
    :server(80), dataBuffer(buffer)
{
}

void MakapoWebServer::begin() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });

  server.on(UriBraces("{}/{}"), HTTP_GET, [this](){
    //toInt() returns 0 if argument is not an integer
    int paddlerId = server.pathArg(0).toInt();

    if(!validId(paddlerId) || paddlerId <= 0){
      handleNotFound("Invalid Paddler Id");
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
        handleNotFound();
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
  doc["speed"] = dataBuffer.getPaddlerDataById(id).speed;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleLocation(int id) {
  StaticJsonDocument<128> doc;
  doc["location"] = dataBuffer.getPaddlerDataById(id).location;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleStrokeRate(int id) {
  StaticJsonDocument<128> doc;
  doc["strokeRate"] = dataBuffer.getPaddlerDataById(id).strokeRate;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handleAvgStrokeForce(int id) {
  StaticJsonDocument<128> doc;
  doc["avgStrokeForce"] = dataBuffer.getPaddlerDataById(id).avgStrokeForce;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

void MakapoWebServer::handlePaddlerData(int id) {
  StaticJsonDocument<512> doc;
  PaddlerData data = dataBuffer.getPaddlerDataById(id);

  doc["speed"] = data.speed;
  doc["location"] = data.location;
  doc["strokeRate"] = data.strokeRate;
  doc["avgStrokeForce"] = data.avgStrokeForce;

  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body);
}

bool MakapoWebServer::validId(int paddlerId){
  return true;
}

void MakapoWebServer::handleNotFound(String error = "This endpoint doesn't exist") {
  StaticJsonDocument<128> doc;
  doc["error"] = error;

  String body;
  serializeJson(doc, body);
  server.send(404, "application/json", body);
}
