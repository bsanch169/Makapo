# include <RadioLib.h>
# include <Arduino.h>
# include "semphr.h"
# include "RawPacket.h"
# include "MakapoRadio.h"
# include "MakapoWebServer.h"

PaddlerDataBuffer liveStorage;
SemaphoreHandle_t mutex;

volatile bool radioMode = false; // false=recv, true=transmit (priority) 

void setup(){
	Serial.begin(115200);
	mutex = xSemaphoreCreateMutex();

	static MakapoSharedParams taskParameters = {
		.liveStorage = &liveStorage,
		.mutex = mutex,
		.radioMode = &radioMode,
	};

	BaseType_t radioTaskStatus = xTaskCreatePinnedToCore(
	    handleRadio,   		// Task function
	    "MkpHubRadio",      // Task name
	    8192,        		// Stack size
	    &taskParameters,  	// Task parameters
    	5,    				// Priority
    	NULL,             	// Task handle
    	0           		// Core on which to run the task
	);
	Serial.print("radio task create result: ");
	Serial.println(radioTaskStatus);

	/*BaseType_t serverTaskStatus = xTaskCreatePinnedToCore(
	    handleClientTask,   // Task function
	    "MkpHubServer",     // Task name
	    8192,        		// Stack size
	    &taskParameters,    // Task parameters
    	5,     				// Priority
    	NULL,             	// Task handle
    	1 		          	// Core on which to run the task
	);

	Serial.print("server task create result: ");
	Serial.println(serverTaskStatus);*/

}

void loop(){
	Serial.println(liveStorage.toString());
	delay(5000);
}

