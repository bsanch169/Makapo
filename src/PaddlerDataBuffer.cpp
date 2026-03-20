#include "PaddlerDataBuffer.h"
#include "RingBuffer.h"

PaddlerDataBuffer::PaddlerDataBuffer() {
}

PaddlerData PaddlerDataBuffer::getPaddlerDataById(int id) {
    PaddlerData data;
    data.paddlerId = id;

    if (id == 1) {
        data.speed = 4.8;
        data.location = "Lane 1";
        data.strokeRate = 32;
        data.avgStrokeForce = 78;
    } 
    else if (id == 2) {
        data.speed = 5.3;
        data.location = "Lane 2";
        data.strokeRate = 35;
        data.avgStrokeForce = 84;
    } 
    else if (id == 3) {
        data.speed = 4.5;
        data.location = "Lane 3";
        data.strokeRate = 29;
        data.avgStrokeForce = 73;
    } 
    else {
        data.speed = 3.9;
        data.location = "Unknown";
        data.strokeRate = 25;
        data.avgStrokeForce = 65;
    }

    return data;
}