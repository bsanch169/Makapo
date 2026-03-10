# ifndef RAWPACKET_H
# define RAWPACKET_H

# include <cstdint>
# include <cstddef>


struct Paddler {
	uint8_t paddlerID;
	uint8_t	paddleAngle;
	uint8_t paddleVelocity;
	uint8_t paddlePressure;
};

struct RawPacket {	
	uint8_t boatID;
	float timestamp;
	uint8_t direction;
	float coordLat;
	float coordLong;
	float speed;

	uint8_t pCount;
	Paddler paddlers[];
};

RawPacket* allocPacket(uint8_t count);
void freePacket(RawPacket* p);
RawPacket* generatePacket(uint8_t count, bool empty = false);

#endif
