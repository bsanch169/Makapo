# include "RawPacket.h"
# include <Arduino.h>
# include <PaddlerDataBuffer.h>


/* Packets are stored and updated as live docs; no history. 
 * As such, we pass an array of pointers through to check the ID & sizes
 * if a slot is used & has correct size, update the data accordingly
 * if not, allocate the memory, assign the pointer to the slot, and fill
 */
bool storeData(PaddlerDataBuffer* live, uint8_t* buffer, uint8_t bufSize){

	//parse through 1st byte to identify boat & pCount; later add flags
	uint8_t identifier = buffer[0];

	uint8_t boatID = (identifier >> 4) & 0x0F;

	if(boatID > 9) return false;

	uint8_t encCount = ((identifier >> 2) & 0x03); 
	uint8_t count = encCount < 1 ? 1 : encCount << 1;

	//validate buffer size matches expected count
	if(	!((bufSize == 17 && count == 1) || 
	    (bufSize == 22 && count == 2) || 
	    (bufSize == 32 && count == 4) || 
	    (bufSize == 42 && count == 6))) return false;

	BoatData update = {};
	update.boatID = boatID;
	update.paddlerCount = count;
	update.senStatus = 0;
    update.boatStatus = 0;

	//update boat data
	memcpy(&update.coordLat, buffer + 1, sizeof(float));
	memcpy(&update.coordLon, buffer + 5, sizeof(float));
	update.speed = buffer[9];
	memcpy(&update.videoID, buffer + 10, sizeof(uint16_t));

	//update paddler data
	uint8_t offset = 12;
	for(int i = 0; i < count; i++){
		update.paddlers[i].paddlerID = buffer[offset++];		
		update.paddlers[i].paddleAng = buffer[offset++];
		update.paddlers[i].paddleVel = buffer[offset++];
		update.paddlers[i].paddlePres = buffer[offset++];
		update.paddlers[i].strokeRate = buffer[offset++];
	}

	live->addBoatData(update);
	return true;
}

//memory handling of packet with dynamic array
static inline size_t rawPacketBytes(size_t count){
	return offsetof(RawPacket, paddlers) + count * sizeof(Paddler);
}	

uint8_t packHeader(uint8_t boatID, uint8_t pCount, uint8_t senStatus, uint8_t boatStatus){
	return 	((boatID & 0x0F) << 4) | 
			((pCount & 0x03) << 2) | 
			((senStatus & 0x01) << 1) | 
			(boatStatus & 0x01);
}