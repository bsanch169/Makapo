# ifndef RAWPACKET_H
# define RAWPACKET_H

# include <cstdint>
# include <cstddef>
# include "PaddlerDataBuffer.h"

/* LoRa Packets used are explicit; Header contains CRC & Payload Size
 * 	Header CRC checks size matches expected on receive; 
 * 	Payload contains CRC (set to 2 bytes on Tx/Rx)
 *
 * 	EP will place data into byte array & transmit
 * 	This packet structure is used to organize & store data Hub-side
 */

/* Byte Configuration within Packets:
 *	Header begins @ Byte[0]:
 *		BoatID: 4 bits
 *		PCount: 2 bits
 *		SensorStatus: 1 bit
 *		BoatStatus: 1 bit, ends @ Byte[1]
 *	Boat MetaData Begins @ Byte[1]
 *		CoordLat @ Byte[1-4]
 *		CoordLong @ Byte[5-8]
 *		Speed @ Byte[9]
 *		VideoID @ Byte[10-11]
 * 	Paddler Data begins at Byte[12]
 * 	5 Byte Length[12-16] -> 5 * N bytes total;
 * 		PaddlerID @ Byte[12]
 * 		PaddleAngle @ Byte[13]
 * 		PaddleVelocity @ Byte[14]
 * 		PaddlePressure @ Byte[15]
 * 		StrokeRate @ Byte[16]
 *
 *  Paddlers unused, packet is 12 bytes
 * 	Once paddle integrated, packet is 17 bytes min - 40 bytes max
 */

/* 
 * Currently no Makapo Identifier; Little to no chance another LoRa device is
	return 	((boatID & 0x0F) << 4) | 
			((pCount & 0x03) << 2) | 
			((senStatus & 0x01) << 1) | 
			(boatStatus & 0x01); * 	on the water, let alone using the same SyncWord
 * 
 * Header: 1 byte
 * 	BoatID: 4 bits, 10 Boats max, correspond to array index position
 * 	PCount: 2 bit mode; 1, 2, 4, or 6 paddlers
 * 	SensorStatus: 1 bit, if there is a sensor issue (to be expanded on)
 * 	BoatStatus: 1 bit, if there is a boat / echopaddle issue (to be expanded on)
 * 
 * BoatData: 9 bytes
 * 	Lat: Float, from EP GPS module
 * 	Long: Float, from EP GPS module
 * 	Speed: Calculated on EP and passed
 *
 * Currently Unused while Paddle EE/ME Team build
 * Paddlers* []: Dynamic Array, 5 Bytes/Paddler; Each field is uint8_t for now
 *  PaddlerID: Small DB, unique IDs; else corresponding to boat position; app maps
 *	PaddleAngle
 *	PaddleVelocity
 *	PaddlePressure 
 *	StrokeRate
 */

struct Paddler {
	uint8_t paddlerID;
	uint8_t	paddleAng;
	uint8_t paddleVel;
	uint8_t paddlePres;
	uint8_t strokeRate;
};

struct RawPacket {	/* float - 32 bits (4 bytes); 9 bytes total boatData */
	/* The head byte when transmitted is dropped; 
	 * The boat id and flags are used during 
	 * 	updates to the live structs;
	 * Instead, this first byte will be used to save the pCount
	 * 	once stored to confirm the next updating packets are consistent
	 */

	/* bit-packed header; EP uses packHeader to fit into 1 byte and transmit */
	uint8_t headByte;

	float coordLat;
	float coordLon;
	uint8_t speed;
	uint16_t videoID;

	Paddler paddlers[]; // comment out or just null paddler until sensor team has deliverable
};

// scaled integers for coord / angle?
uint8_t packHeader(uint8_t boatID, uint8_t pCount, uint8_t senStatus, uint8_t boatStatus);
bool storeData(PaddlerDataBuffer* live, uint8_t* buffer, uint8_t bufSize);
#endif
