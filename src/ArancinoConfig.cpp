/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2021 SmartMe.IO

Authors:  Sergio Tomasello <sergio@smartme.io>

Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations
under the License
*/

#include "ArancinoConfig.h"
#include "ArancinoDefinitions.h"
#include <Arduino.h>

/*
    All the interface specific methods should be placed here
*/

/******** Serial interface *********/

void SerialIface::ifaceBegin(){
    SERIAL_PORT.begin(BAUDRATE);
    SERIAL_PORT.setTimeout(SERIAL_TIMEOUT); 
}

void SerialIface::sendArancinoCommand(char* command){
    //check communication timeout with arancino module
	if (comm_timeout){
		/*
			Flush data on serial communication to avoid of lost
			synchronization between arancino library and arancino module.
			By this way I prevent to receive reposonse of a previous sent command.
		*/
		while(SERIAL_PORT.available() > 0){
				SERIAL_PORT.read();
		}
		comm_timeout=false;
	}
	//command must terminate with '\0'!
	SERIAL_PORT.write(command, strlen(command)); //excluded '\0'
	#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			if(command[strlen(command) - 1] == END_TX_CHAR)
			{
				Serial.println(command);
			}
			else
				Serial.print(command);
		}
	#endif
}

char* SerialIface::receiveArancinoResponse(char terminator){
    char* response = NULL; //must be freed
	String str = "";
	str = SERIAL_PORT.readStringUntil(terminator);
	if( str == ""){
		//enable timeout check
		comm_timeout = true;
	}
	else {
		int responseLength = strlen(str.begin());
		if (responseLength > 0)
		{
			response = (char *)calloc(responseLength + 1 + 1, sizeof(char));
			strcpy(response, str.begin());
			response[responseLength] = END_TX_CHAR;
			response[responseLength + 1] = '\0';
		}
	}
	return response;
}