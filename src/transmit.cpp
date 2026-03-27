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

//has receive / transmit been completed?
volatile bool operationDone = false;
void operationFlag(){ operationDone = true; }

//status code saved after recieve/transmit; if != 0, error occured
int radioStatus = RADIOLIB_ERR_NONE;

//assuming hardcoded data like boatID and paddlerCount
uint8_t boatID = 1;
uint8_t pCount = 3; //mode, so 0=1, 1=2, 2=4, 3=6
uint8_t senStatus = 0;
uint8_t boatStatus = 0;

//testing: send a packet every second
long transmissionDelay = 3000; //in ms
long lastTransmission = 0;
uint8_t counter = 0;
uint8_t* payload;

//function prototypes
void display_start();
void setupLoRa();
void printToDisplay(const char* a, const char* b = nullptr, const char* c = nullptr);
uint8_t* prepData();
size_t getPacketLength();
void initRandomSeed();

void setup(){
	heltec_setup();
	display_start();
	initRandomSeed(); //from RawPackets.cpp

	printToDisplay("Board setup success, setting up LoRa...");

	setupLoRa();
	radio.explicitHeader(); /* Sets explicit header for packets (usually default but just in case) */
	radio.setCRC(2); /* Sets 2 byte payload CRC for packet */
	radio.setDio1Action(operationFlag); /* interrupt method on radio action completion i.e transmit/receive/etc */
	
	if(radioStatus == RADIOLIB_ERR_NONE){
		//Just transmit one packet for now.	
		printToDisplay("Generating Random Packet...");
		payload = prepData();
		
		if(payload == nullptr){
			printToDisplay("ERROR: Could not generate data!");
			while(true) { delay(10); }
		}

		size_t payloadLen = getPacketLength();
		radioStatus = radio.startTransmit(payload, payloadLen);
		
		if(radioStatus == RADIOLIB_ERR_NONE) {
			printToDisplay("Transmitted Successfully");
			counter++;
			lastTransmission = millis();
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

	//non-blocking, send a packet at set delay (3 seconds rn)
	if(millis() - lastTransmission > transmissionDelay){
		payload = prepData();
	
		if(payload == nullptr){
			printToDisplay("ERROR: Transmission Data Could Not be Generated!");
			while(true) { delay(10); } 
		}
		size_t payloadLen = getPacketLength();
		radioStatus = radio.startTransmit(payload, payloadLen);
	}

	if(operationDone){
		operationDone = false;
		if(radioStatus == RADIOLIB_ERR_NONE){
			lastTransmission = millis();
			char tCount[12];
			snprintf(tCount, sizeof(tCount), "%u", ++counter);
			printToDisplay("Packets Transmitted: ", tCount);
			free(payload);
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

uint8_t* prepData(){

	//make buffer 
	size_t len = getPacketLength();
	uint8_t* buffer = (uint8_t*)malloc(len);	
	if(buffer == nullptr) {
		heltec_led(30);
		printToDisplay("ERROR: Could not create byte buffer!");
		return nullptr;
	}

	buffer[0] = packHeader(boatID, pCount, senStatus, boatStatus);
	float coordLat = random(0, 64001) / 10.0f;
	float coordLon = random(0, 64001) / 10.0f;
	memcpy(buffer + 1, &coordLat, sizeof(float));
	memcpy(buffer + 5, &coordLon, sizeof(float));
	buffer[9] = random(0, 255);

	uint8_t offset = 10; 
	
	/* 	0000 + 1 = 1
		0001 << 0010 = 2
		0010 << 0100 = 4
		0011 << 0110 = 6   */
	uint8_t paddlers = pCount < 1 ? (pCount << 1) + 1 : pCount << 1;
	for(int i = 0; i < paddlers; i++){
		buffer[offset++] = random(1,7); //paddler ID
		buffer[offset++] = random(0,255); //paddle angle
		buffer[offset++] = random(0,255); //paddle velocity
		buffer[offset++] = random(0,255); //paddle pressure
		buffer[offset++] = random(0,255); //strokerate
	}
	return buffer;
}

size_t getPacketLength(){
	uint8_t paddlers = pCount < 1 ? (pCount << 1) + 1 : pCount << 1;
	return 10 + (size_t)paddlers * 5; // header + boatdata + count * 5
}

void initRandomSeed(){
	randomSeed(esp_random() ^ millis() ^ micros());
}
