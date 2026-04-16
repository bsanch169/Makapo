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

bool PaddlerDataBuffer::addBoatData(uint8_t boatId, float speed, float longitude, float latitude,
                                    PaddlerData* paddlerData, uint8_t paddlerCount) {

  latestState[boatId].boatId = boatId;
  latestState[boatId].speed = speed;
  latestState[boatId].longitude = longitude;
  latestState[boatId].latitude = latitude;

  if (paddlerData != nullptr) {
    for (int i = 0; i < paddlerCount; i++) {
      latestState[boatId].paddlerData[i] = paddlerData[i];
    }
  }

  if (!hasLatest[boatId]) {
    hasLatest[boatId] = true;
    ++numCanoes;
  }
}

bool PaddlerDataBuffer::getLatestBoatData(uint8_t boatId, BoatData& out) {
  if (!hasData(boatId)) return false;
  if (!hasLatest[boatId]) return false;

  return true;
}