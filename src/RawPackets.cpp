# include "RawPackets.h"
# include <Arduino.h>

RawPacket* generatePacket(uint8_t count, bool empty){
	
	//allocate memory for packet with dynamic array
	RawPacket* p = allocPacket(count);
	if(!p) return nullptr;
	if(empty) return p;

	//seed randnum generator
	randomSeed(0);

	//generate boat data
	p->boatID = random(0,6);
	p->timestamp = random(1, 10001) / 1000.0;
	p->direction = random(0,3);
	p->coordLat = random(1, 10001) / 1000.0;
	p->coordLong = random(1, 10001) / 1000.0;
	p->speed = random(1, 60) / 2.0;
	
	//populate array
	for(int i = 0; i < count; i++){
		p->paddlers[i].paddlerID = i;		
		p->paddlers[i].paddleAngle = random(1,180);
		p->paddlers[i].paddleVelocity = random(1,25);
		p->paddlers[i].paddlePressure = random(1,100);
	}
	
	return p;
}

//memory handling of packet with dynamic array
static inline size_t rawPacketBytes(size_t count){
	return offsetof(RawPacket, paddlers) + count * sizeof(Paddler);
}

RawPacket* allocPacket(uint8_t count){
	RawPacket* p = (RawPacket*) malloc(rawPacketBytes(count));
	if(!p) return nullptr;
	
	p->pCount = count;
	return p;
}

void freePacket(RawPacket* p) {
	free(p);
}
