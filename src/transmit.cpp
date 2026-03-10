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

//has receive / transmit been completed?
volatile bool operationDone = false;

void operationFlag(){ operationDone = true; }

RawPacket* packet;

//status code saved after recieve/transmit; if != 0, error occured
int radioStatus = RADIOLIB_ERR_NONE;

//function prototypes
void display_start();
void setupLoRa();
void printToDisplay(const char* a, const char* b = nullptr, const char* c = nullptr);
size_t getPacketLength(RawPacket* packet);
void displayPacket(RawPacket* packet);
uint8_t* packetToBytes(RawPacket* packet);

void setup(){
	heltec_setup();
	display_start();

	printToDisplay("Board setup success, setting up LoRa...");

	setupLoRa();
	radioStatus = radio.explicitHeader();
	radio.setDio1Action(operationFlag);
	
	if(radioStatus == RADIOLIB_ERR_NONE){
		//Just transmit one packet for now.	
		printToDisplay("Generating Random Packet...");
		packet = generatePacket(random(1,7));

		if(!packet){
			printToDisplay("Packet Gen Error!");
			while(true) { delay(10); }
		}
		
		printToDisplay("Packet created, preparing for transmit...");
		uint8_t* payload = packetToBytes(packet);
		if(payload == nullptr){
			printToDisplay("Failed to convert packet to payload!");
			while(true) { delay(10); }
		}

		size_t packetLen = getPacketLength(packet);

		radioStatus = radio.startTransmit(payload, packetLen);
		
		if(radioStatus == RADIOLIB_ERR_NONE) {
			printToDisplay("Packet Transmitted Successfully");
			free(payload);
		}
		else{
			char errBuf[16];
			snprintf(errBuf, sizeof(errBuf), "%d", radioStatus);
			printToDisplay("Error Transmitting: ", errBuf);
			while(true) { delay(10); }
		}
	}
}

void loop(){
	heltec_loop();

	if(operationDone){
		operationDone = false;
		if(radioStatus == RADIOLIB_ERR_NONE){
			printToDisplay("Transmission success!, cleaning up...");
			radioStatus = radio.finishTransmit();

			printToDisplay("Starting Packet Display Loop...");
			displayPacket(packet);
		}
		else{
			char errBuf[16];
			snprintf(errBuf, sizeof(errBuf), "%d", radioStatus);
			printToDisplay("Error during transmission:", errBuf);
			while(true) { delay(10); }
		}
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

	delay(500);
}

void setupLoRa(){
	radioStatus = radio.begin(FREQ, BANDWIDTH, SPREAD_FACTOR, CODE_FACTOR, SYNCWORD, PREAM_LEN);

	if(radioStatus == RADIOLIB_ERR_NONE){
		printToDisplay("LoRa Init Success");
	}
	else{
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

//definitely refine the position finding later...
uint8_t* packetToBytes(RawPacket* packet){
	size_t len = getPacketLength(packet);
	uint8_t* buffer = (uint8_t*)malloc(len);	
	if(buffer == nullptr) {
		heltec_led(30);
		printToDisplay("ERROR: Could not create byte buffer!");
		return nullptr;
	}

	buffer[0] = packet->boatID;
	memcpy(buffer + 1, &packet->timestamp, sizeof(float));
	buffer[5] = packet->direction;
	memcpy(buffer + 6, &packet->coordLat, sizeof(float));
	memcpy(buffer + 10, &packet->coordLong, sizeof(float));
	memcpy(buffer + 14, &packet->speed, sizeof(float));
	buffer[18] = packet->pCount;

	uint8_t count = packet->pCount;
	uint8_t offset = 19;
	for(int i = 0; i < count; i++){
		buffer[offset++] = packet->paddlers[i].paddlerID;
		buffer[offset++] = packet->paddlers[i].paddleAngle;
		buffer[offset++] = packet->paddlers[i].paddleVelocity;
		buffer[offset++] = packet->paddlers[i].paddlePressure;
	}
	return buffer;
}

size_t getPacketLength(RawPacket* packet){
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
