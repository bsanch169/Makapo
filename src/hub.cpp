# include <RadioLib.h>
# include <Arduino.h>
# include "semphr.h"
# include "RawPacket.h"
# include "MakapoRadio.h"

PaddlerDataBuffer liveStorage;
SemaphoreHandle_t mutex;
volatile bool radioMode = false; // false=recv, true=transmit (priority) 
void setup(){
	mutex = xSemaphoreCreateMutex();

	static MakapoSharedParams taskParameters = {
		.liveStorage = &liveStorage,
		.mutex = mutex,
		.radioMode = &radioMode,
	};

	xTaskCreatePinnedToCore(
	    handleRadio,   		// Task function
	    "MkpHubRadio",      // Task name
	    4096,        		// Stack size
	    &taskParameters,  	// Task parameters
    	5,    				// Priority
    	NULL,             	// Task handle
    	0           		// Core on which to run the task
	);
	/*
	xTaskCreatePinnedToCore(
	    handleClient,   	// Task function
	    "MkpHubServer",     // Task name
	    4096,        		// Stack size
	    &taskParameters,    // Task parameters
    	5,     				// Priority
    	NULL,             	// Task handle
    	1 		          	// Core on which to run the task
	);
	*/
}

void loop(){}
