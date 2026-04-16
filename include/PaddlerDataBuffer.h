#pragma once 
#include "Arduino.h"

//Max number of canoes, max paddlers per canoe
constexpr size_t MAX_CANOES = 10;
constexpr size_t MAX_PADDLERS = 6;


struct PaddlerData{
	uint8_t paddlerID;
	uint8_t	paddleAng;
	uint8_t paddleVel;
	uint8_t paddlePres;
	uint8_t strokeRate;
};

struct BoatData{
  uint8_t boatId;
  float speed;     
  float longitude;
  float latitude;
  uint8_t paddlerCount;
  PaddlerData paddlerData[MAX_PADDLERS];
};


class PaddlerDataBuffer{
public:
  PaddlerDataBuffer();

  bool getLatestBoatData(uint8_t boatId, BoatData& out);
  bool addBoatData(uint8_t boatId, float speed, float longitude, float latitude, 
                    PaddlerData *paddlerData, uint8_t paddlerCount);
  bool hasData(uint8_t boatId);


private:
  uint8_t numCanoes;
  bool hasLatest[MAX_CANOES];
  BoatData latestState[MAX_CANOES];
};  

