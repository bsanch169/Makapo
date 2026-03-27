# include <RadioLib.h>
# include <Arduino.h>
# include <heltec_unofficial.h> //shorthand display/radio init, radiolib functions

# include "RawPacket.h"

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

//live storage & boat tracking
RawPacket* liveStorage[10] = {};
uint8_t liveConnections = 0;

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
		if(recvLen > 10) {
			printToDisplay("Data Packet Received...");
		}

		uint8_t* rawBytes = (uint8_t*)malloc(recvLen);
		if(rawBytes == nullptr){
			while(true) { delay(10); }
		}

		//receive and store into raw byte buffer
		if(radio.readData(rawBytes, recvLen) == RADIOLIB_ERR_NONE){
			uint8_t header = rawBytes[0];
			if(liveStorage[(header >> 4) & 0x0F] == nullptr){
				printToDisplay("New Boat Connection!");
			}

			printToDisplay("Packet Received!", "Unpacking...");
			RawPacket* newPacket = storeData(liveStorage, rawBytes, recvLen);
			if(newPacket == nullptr) while(true) { delay(10); }
		}
		else{
			printToDisplay("Error reading packet!");
			while(true) { delay(10); }
		}	

		free(rawBytes);
		radioStatus = radio.startReceive();
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

	/* Should allocate the space for 10 buffers corresponding 
	 * to per-boat packet sizes; But realistically, we only get location 	 * data at the moment (no paddle sensor deliverable) and we're not 
	 * doing historical data or calculations so we'll simply have 10 max sized Packet Structs that are going to be updated on a receive; 
	 * [Packet Collisions and Access between Tasks to build later]
	 */
	
	/* has to be an array of pointers, pointing to packets of certain size; on reception, the space will be allocated, index tied to boatID, and updated accordingly */

	/* or can be preallocated memory pool of each slot max size, but i don't wanna deal with memory stuff rn tbh */

