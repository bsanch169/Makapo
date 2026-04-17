#include "PaddlerDataBuffer.h"

PaddlerDataBuffer::PaddlerDataBuffer() {
  numCanoes = 0;
  for (size_t i = 0; i < MAX_CANOES; ++i) {
    hasLatest[i] = false;
  }
}

bool PaddlerDataBuffer::hasData(uint8_t boatId) {
  if (boatId >= MAX_CANOES) return false;
  return hasLatest[boatId];
}

bool PaddlerDataBuffer::addBoatData(BoatData data) {
  if(data.boatID > MAX_CANOES) return false;

  latestState[data.boatID] = data;
  hasLatest[data.boatID] = true;
  return true;
}

bool PaddlerDataBuffer::getLatestBoatData(uint8_t boatId, BoatData& out) {
  if (!hasData(boatId)) return false;
  out = latestState[boatId]; 

  return true;
}

String PaddlerDataBuffer::printData() {
  String out = "";

  for (int i = 0; i < MAX_CANOES; i++) {
    if (!hasLatest[i]) continue;

    BoatData& b = latestState[i];

    out += "Boat ";
    out += String(b.boatID);
    out += " | Speed: ";
    out += String(b.speed);
    out += " | Lat: ";
    out += String(b.coordLat, 6);
    out += " | Lon: ";
    out += String(b.coordLon, 6);
    out += " | VideoID: ";
    out += String(b.videoID);
    out += "\n";

    for (int j = 0; j < b.paddlerCount; j++) {
      PaddlerData& p = b.paddlers[j];

      out += "  Paddler ";
      out += String(p.paddlerID);
      out += " | Paddle Angle: ";
      out += String(p.paddleAng);
      out += " | Paddle Velocity: ";
      out += String(p.paddleVel);
      out += " | Paddle Pressure: ";
      out += String(p.paddlePres);
      out += " | Stroke Rate: ";
      out += String(p.strokeRate);
      out += "\n";
    }
    out += "\n";
  }

  return out;
}