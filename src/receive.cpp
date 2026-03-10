# include <RadioLib.h>
# include <Arduino.h>
# include <heltec_unofficial.h> //shorthand display/radio init, radiolib functions

# include "RawPackets.h"

//transmitter initialization
static float FREQ = 915.0;
static float BANDWIDTH = 125.0;
static uint8_t SPREAD_FACTOR = 9;
static uint8_t CODE_FACTOR = 7;
static uint8_t SYNCWORD = 38;
static uint16_t PREAM_LEN = 8;

//transmit / receive state
volatile bool operationDone = false;
void operationFlag(){ operationDone = true; }

//status code saved after recieve/transmit; if != 0, error occured
int radioStatus = RADIOLIB_ERR_NONE;

//function prototypes
void display_start();
void setupLoRa();
void printToDisplay(const char* a, const char* b = nullptr, const char* c = nullptr);
size_t getPacketLength(RawPacket* packet);
void displayPacket(RawPacket* packet);
void bytesToPacket(uint8_t* buffer, RawPacket* empty);

//temporary storage for raw byte data
uint8_t* buffer;

void setup(){
	heltec_setup();
	display_start();

	printToDisplay("Board setup success, setting up LoRa...");

	setupLoRa();
	radioStatus = radio.explicitHeader();
	radio.setDio1Action(operationFlag);

	if(radioStatus == RADIOLIB_ERR_NONE){
		printToDisplay("LoRa init successful!");

		printToDisplay("Listening for Test Packet");
		radioStatus = radio.startReceive();
	}
}

void loop(){
	heltec_loop();

	if(operationDone){
		operationDone = false;

		//get length of packet from lora, create rawbyte buffer
		size_t recvLen = radio.getPacketLength();
		if(recvLen < 19) {
			printToDisplay("Received Packet size incorrect!");
			while(true) { delay(10); }
		}

		uint8_t* rawBytes = (uint8_t*)malloc(recvLen);
		if(rawBytes == nullptr){
			printToDisplay("Failed to allocate memory for byte buffer!");
			while(true) { delay(10); }
		}

		//receive and store into raw byte buffer
		if(radio.readData(rawBytes, recvLen) == RADIOLIB_ERR_NONE){
			printToDisplay("Test Packet Received!", "Unpacking...");
		}
		else{
			printToDisplay("Error reading packet!");
			while(true) { delay(10); }
		}
		
		//check length of paddler section (pcount)
		uint8_t pCount = rawBytes[18];

		//allocate memory for rawpacket struct
		RawPacket* parsed = generatePacket(pCount, true); //empty packet
		
		//verify size of received & container
		size_t packLen = getPacketLength(parsed);
		if(recvLen < packLen) {
			printToDisplay("ERROR: Received Packet does not match Generated Packet size!");
			while(true) { delay(10); }
		}

		/* parse bytes into packet structure;
		   method takes ptr to temp buffer and empty packet */
		bytesToPacket(rawBytes, parsed);
		free(rawBytes);

		//display received packet
		displayPacket(parsed);
	}

}

void printToDisplay(const char* a, const char* b, const char* c){
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	int x = 0;
	int y = 0; 
	int lineH = 8;

	display.drawString(x, y, a); y += lineH;
	if(b) display.drawString(x, y, b); y += lineH;
	if(c) display.drawString(x, y, c); y += lineH;

	display.display();

	delay(1500);
}

void setupLoRa(){
	radioStatus = radio.begin(FREQ, BANDWIDTH, SPREAD_FACTOR, CODE_FACTOR, SYNCWORD, PREAM_LEN);

	if(radioStatus == RADIOLIB_ERR_NONE){
		printToDisplay("LoRa Init Success");
	}
	else {
		char errBuf[16];
		snprintf(errBuf, sizeof(errBuf), "%d", radioStatus);
		printToDisplay("LoRa Failure: ", errBuf);

		while(true) { delay(10); }
	}
}

void display_start(){
	//heltec_setup sets up display, this provides it power and initalizes it
	pinMode(Vext, OUTPUT); 
	digitalWrite(Vext, LOW);
	delay(50);

	display.init();
	display.flipScreenVertically();
	display.clear();
	display.display();
}
void bytesToPacket(uint8_t* rawBuffer, RawPacket* packet){
	packet->boatID = rawBuffer[0];
	memcpy(&packet->timestamp, rawBuffer + 1, sizeof(float)); //I THINK???
	packet->direction = rawBuffer[5];
	memcpy(&packet->coordLat, rawBuffer + 6, sizeof(float));
	memcpy(&packet->coordLong, rawBuffer + 10, sizeof(float));
	memcpy(&packet->speed, rawBuffer + 14, sizeof(float));
	uint8_t count = packet->pCount = rawBuffer[18];

	uint8_t offset = 19;
	for(int i = 0; i < count; i++){
		packet->paddlers[i].paddlerID = rawBuffer[offset++];
		packet->paddlers[i].paddleAngle = rawBuffer[offset++];
		packet->paddlers[i].paddleVelocity = rawBuffer[offset++];
		packet->paddlers[i].paddlePressure = rawBuffer[offset++];
	}
}

size_t getPacketLength(RawPacket* packet){
	/* add these definitions to top of file once data selection is finalized
	size_t WIRE_HEADER_LEN = 19;
	size_t PADDLER_WIRE_LEN = 4;
	*/

	return 19 + (size_t)packet->pCount * 4;
}

void displayPacket(RawPacket* packet){
	char a[32], b[32], c[32];

	while(true){
		snprintf(a, sizeof(a), "boatID: %u", packet->boatID);
		snprintf(b, sizeof(b), "ts: %.3f", packet->timestamp);
		snprintf(c, sizeof(c), "dir: %u", packet->direction);
		printToDisplay(a,b,c);
		delay(3000);

		snprintf(a, sizeof(a), "lat: %.3f", packet->coordLat);
		snprintf(b, sizeof(b), "lon: %.3f", packet->coordLong);
		snprintf(c, sizeof(c), "spd: %.2f", packet->speed);
		printToDisplay(a,b,c);
		delay(3000);

		for(uint8_t i = 0; i < packet->pCount; i++){
			snprintf(a, sizeof(a), "P%u id: %u", i, packet->paddlers[i].paddlerID);
			snprintf(b, sizeof(b), "ang: %u vel %u", packet->paddlers[i].paddleAngle, packet->paddlers[i].paddleVelocity);
			snprintf(c, sizeof(c), "pres: %u", packet->paddlers[i].paddlePressure);
			printToDisplay(a,b,c);
			delay(3000);	
		}
	}
}
