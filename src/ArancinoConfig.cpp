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
#include <Arancino.h>

/*
    All the interface specific methods should be placed here
*/

/******** Serial interface *********/

#if defined(ARANCINO_SERIAL_IFACE)

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
			response = (char *)Arancino.calloc(responseLength + 1 + 1, sizeof(char));
			strcpy(response, str.begin());
			response[responseLength] = END_TX_CHAR;
			response[responseLength + 1] = '\0';
		}
	}
	return response;
}


/******** MQTT interface *********/

#elif defined(ARANCINO_MQTT_IFACE)

//Static variables definition
char* MqttIface::_inputBuffer;
bool MqttIface::_newIncomingMessage = false;
char* MqttIface::_inputTopic;
char* MqttIface::_outputTopic;
char* MqttIface::_serviceTopic;

void MqttIface::setNetworkClient(Client* networkClient){
	this->_client = networkClient;
}

void MqttIface::ifaceBegin(){
	setClient(*client);
	setServer(broker, port);
	setCallback(MqttIface::_arancinoCallback);

	//+2 cause 1 '/' is missing in the calculation other than \n
	MqttIface::_inputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(daemonID) + strlen(Arancino.id) + strlen("/rsp_to_mcu") + 2, sizeof(char));
	MqttIface::_outputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(daemonID) + strlen(Arancino.id) + strlen("/cmd_from_mcu") + 2, sizeof(char));
	strcpy(MqttIface::_inputTopic, "arancino/cortex/");
	strcat(MqttIface::_inputTopic, daemonID);
	strcat(MqttIface::_inputTopic, "/");
	strcat(MqttIface::_inputTopic, Arancino.id);

	strcpy(MqttIface::_outputTopic, MqttIface::_inputTopic);	//just a quick shortcut
	strcat(MqttIface::_inputTopic, "/rsp_to_mcu");
	strcat(MqttIface::_outputTopic, "/cmd_from_mcu");


	while (!this->connected()){
		if (this->connect(Arancino.id, username, password)){
			char* discoverytopic = (char*)Arancino.calloc(strlen("arancino/discovery/") + strlen(daemonID)+1, sizeof(char));
			strcpy(discoverytopic, "arancino/discovery/");
			strcat(discoverytopic, daemonID);
			this->publish(discoverytopic, Arancino.id);
			Arancino.free(discoverytopic);
		}
	}

	MqttIface::_serviceTopic = (char*)Arancino.calloc(strlen("arancino/service/")+ strlen(Arancino.id) + 1, sizeof(char));
	strcpy(MqttIface::_serviceTopic, "arancino/service/");
	strcat(MqttIface::_serviceTopic, Arancino.id);

	this->subscribe(MqttIface::_serviceTopic);
	this->subscribe(MqttIface::_inputTopic);
}

void MqttIface::sendArancinoCommand(char* command){
	this->publish(MqttIface::_outputTopic, command);
}

char* MqttIface::receiveArancinoResponse(char terminator){
	int counter = 0;
	while(!MqttIface::_newIncomingMessage){
		if (counter < MQTT_MAX_RETRIES){
			this->loop();
			counter++;
			delay(10);
		} else {
			//No need for cleanup: no message was received nor memory allocated for it
			return NULL;
		}
	}

	//Clean this before going out. _inputBuffer will be freed by caller function
	MqttIface::_newIncomingMessage = false;
	return MqttIface::_inputBuffer;
}

void MqttIface::_arancinoCallback(char* topic, byte* payload, unsigned int length){
	MqttIface::_inputBuffer = (char*)Arancino.calloc(length+1+1, sizeof(char));
	MqttIface::_inputBuffer = (char*)payload; //I can just cast it to char*

	if (strcmp(topic, "arancino/service") == 0){
		//Arancino.systemReset();
	} else if (strcmp(topic, _inputTopic) == 0){
		MqttIface::_inputBuffer[length] = END_TX_CHAR;
		MqttIface::_inputBuffer[lenght+1] = '\0';
		MqttIface::_newIncomingMessage = true;
	} 
}

#elif defined(ARANCINO_BLUETOOTH_IFACE)

void BluetoothIface::setBLESerial(Stream* bleUart){
	this->_bleSerial = bleUart;
}

void BluetoothIface::ifaceBegin(){
	//Nothing to initialize here. BLEuart should be already initialized when passed to Arancino library
}

void BluetoothIface::sendArancinoCommand(char* command){
	if (comm_timeout){
		while (_bleSerial->available() > 0){
			_bleSerial->read();
		}
		comm_timeout = false;
	}

	_bleSerial->write(command, strlen(command));
}

char* BluetoothIface::receiveArancinoResponse(char terminator){
	char* response = NULL;
	String str = "";
	str = _bleSerial->readStringUntil(terminator);

	//Check timeout
	if (str == ""){
		comm_timeout = true;
	} else {
		int responseLenght = strlen(str.begin());
		if (responseLenght > 0){
			response = (char*) Arancino.calloc(responseLenght+1+1, sizeof(char));
			strcpy(response, str.begin());
			response[responseLenght] = END_TX_CHAR;
			response[responseLenght+1] = '\0';
		}
	}

	return response; 
}

#endif