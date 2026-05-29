#pragma once 
#include "Arduino.h"

//Max number of canoes, max paddlers per canoe
constexpr size_t MAX_CANOES = 10;
constexpr size_t MAX_PADDLERS = 6;

struct Message {
	uint8_t boatID;
	uint8_t messageID;
	uint8_t messageLen;
	String message;
};

struct PaddlerData{
	uint8_t paddlerID;
	uint8_t	paddleAng;
	uint8_t paddleVel;
	uint8_t paddlePres;
	uint8_t strokeRate;
};

struct BoatData{
  uint8_t boatID;
  uint8_t paddlerCount;
  uint8_t senStatus;
  uint8_t boatStatus;

	float coordLat;
	float coordLon;
	uint8_t speed;
	uint16_t videoID;

  PaddlerData paddlers[MAX_PADDLERS];
};


class PaddlerDataBuffer{
public:
  PaddlerDataBuffer();

  bool getLatestBoatData(uint8_t boatId, BoatData& out);
  bool addBoatData(BoatData data);
  bool addMessage(struct Message);
  bool getMessage(struct Message& msg);
  bool hasData(uint8_t boatId);
  String toString();


private:
  uint8_t numCanoes;
  bool hasLatest[MAX_CANOES];
  BoatData latestState[MAX_CANOES];
  struct Message pendingMsg; 
};  

