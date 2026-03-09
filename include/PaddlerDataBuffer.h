#pragma once 
#include "Arduino.h"
#include "RingBuffer.h"


struct PaddlerData {
  int paddlerId;
  float speed;     
  String location;
  int strokeRate;
  int avgStrokeForce;
};


class PaddlerDataBuffer{
public:
  PaddlerDataBuffer();

  PaddlerData getPaddlerDataById(int id);

private:

};

