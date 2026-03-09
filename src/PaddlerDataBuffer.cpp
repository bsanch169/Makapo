#include "PaddlerDataBuffer.h"
#include "RingBuffer.h"


const int MAX_PADDLERS = 10;
const int QUEUE_SIZE = 10;


PaddlerDataBuffer::PaddlerDataBuffer(){

}

PaddlerData PaddlerDataBuffer::getPaddlerDataById(int id){
    PaddlerData data;

    data.speed = 0;
    data.location = "dummy";
    data.avgStrokeForce = 0;
    data.paddlerId = id;
    data.strokeRate = 0;

    return data;
};


