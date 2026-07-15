# include <RadioLib.h>
# include <Arduino.h>
# include <heltec_unofficial.h> //shorthand display/radio init, radiolib functions

# include "RawPacket.h"
# include "PaddlerDataBuffer.h"

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
bool prepMessageData(Message msg, uint8_t*& payload, size_t& payloadLen);
void display_start();
void setupLoRa();
void printToDisplay(const char* a, const char* b = nullptr, const char* c = nullptr);

void handleRadio(void* pvParameters){
	heltec_setup();
	display_start();

	Serial.println("Board setup success");
	printToDisplay("Board setup success, setting up LoRa...");

	setupLoRa();
	radioStatus = radio.explicitHeader();
	radio.setDio1Action(operationFlag);	

	if(radioStatus == RADIOLIB_ERR_NONE){
		printToDisplay("LoRa init successful!");

		printToDisplay("Listening for Test Packet");
		radioStatus = radio.startReceive();
	}
	
	// Unpack Task Parameters
	auto* params = static_cast<MakapoSharedParams*>(pvParameters);
	PaddlerDataBuffer* liveStorage = params->liveStorage;
	SemaphoreHandle_t mutex = params->mutex;
	volatile bool* mode = params->radioMode; //true=trnsmt, false=recv


	for(;;){
		/* Packet Receiving */
		if(!(*mode) && operationDone){
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
				
				// take mutex to store / update data
				xSemaphoreTake(mutex, portMAX_DELAY);

				if(!liveStorage->hasData((header >> 4) & 0x0F)){
					printToDisplay("New Boat Connection!", "Unpacking...");
				}
				else{
					int value = rawBytes[1];
					char buf[12];
					printToDisplay("Recieved", itoa(value, buf, 10));
				}
				bool success = storeData(liveStorage, rawBytes, recvLen);
				if(!success) while(true) { delay(10); }
				xSemaphoreGive(mutex);

			}
			else{
				printToDisplay("Error reading packet!");
			}
			free(rawBytes);
			radioStatus = radio.startReceive();
		}

		/* Packet Transmitting (Unimplemented) */
		if(*mode){
			Message msg;
			Serial.println("Generated data");

			xSemaphoreTake(mutex, portMAX_DELAY);
			bool gotMessage = liveStorage->getMessage(msg);
			xSemaphoreGive(mutex);

			if(!gotMessage){
                printToDisplay("ERROR: Transmission Data Could Not be Generated!");
                while(true) { delay(10); } 
			}

			uint8_t* payload;
			size_t payloadLen;
			
			prepMessageData(msg, payload, payloadLen);
            radioStatus = radio.startTransmit(payload, payloadLen);

            if(radioStatus == RADIOLIB_ERR_NONE){
				operationDone = false;

            }else{
                char errBuf[16];
                snprintf(errBuf, sizeof(errBuf), "%d", radioStatus);
                printToDisplay("Error during transmission:", errBuf);
            }

			while(!operationDone){
    			vTaskDelay(1);
			}

			operationDone = false;
			*mode = false;
			radioStatus = radio.startReceive();
			printToDisplay("Transmit Done");

			free(payload);
			payload = nullptr;
		}

		vTaskDelay(pdMS_TO_TICKS(1));
	}
}
void printToDisplay(const char* a, const char* b, const char* c){
	display.clear();
	display.setFont(ArialMT_Plain_16);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	int x = 0;
	int y = 0; 
	int lineH = 12;

	display.drawString(x, y, a); y += lineH;
    if (b) {
        display.drawString(x, y, b);
        y += lineH;
    }
    if (c) {
        display.drawString(x, y, c);
        y += lineH;
    }

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

bool prepMessageData(Message msg, uint8_t*& payload, size_t& payloadLen) {
	//Preset message
	if(msg.messageID != 0){
		payload = (uint8_t*) malloc(2);
		if(payload == nullptr){
			printToDisplay("Could not generate packet data!");
			return false;
		}
		payloadLen = 2;


		//First 4 bits = boatID, next 4 = messageID
		uint8_t destBoatID = msg.boatID << 4;
		uint8_t messageID = msg.messageID;
		payload[0] = destBoatID | messageID;

		//Next byte is message length (0 for preset messages)
		payload[1] = 0;

	//Custom message
	}else{
		String message = msg.message;
		size_t msgLen = message.length();
		
		//Payload is first 2 bytes + size of custom message
		payload = (uint8_t*) malloc(2 + msgLen);
		if(payload == nullptr){
			printToDisplay("Could not generate packet data!");
			return false;
		}
		payloadLen = 2 + msgLen;

		//No need to check messageID, we know its 0 for custom messages
		payload[0] = msg.boatID << 4;
		payload[1] = msgLen;

		//Adding custom message to payload bytes
		for(int i = 0; i <  msg.message.length(); i++){
			payload[i + 2] = message.charAt(i);
		}
	}
    return true;
}
