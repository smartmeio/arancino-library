/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2022 SmartMe.IO

Authors:  Marco Calapristi <marco.calapristi@smartme.io>

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

#include "ArancinoInterface.h"
#include <Arancino.h>

/*
    All the interface specific methods should be placed here
*/

/******** Serial interface *********/

void SerialIface::ifaceBegin(){
    //Nothing to initialize here.
}

void SerialIface::sendArancinoCommand(char* command){
    //check communication timeout with arancino module
	if (comm_timeout){
		/*
			Flush data on serial communication to avoid of lost
			synchronization between arancino library and arancino module.
			By this way I prevent to receive reposonse of a previous sent command.
		*/
		while(this->_serialPort->available() > 0){
				this->_serialPort->read();
		}
		comm_timeout=false;
	}
	//command must terminate with '\0'!
	this->_serialPort->write(command, strlen(command)); //excluded '\0'
}

char* SerialIface::receiveArancinoResponse(char terminator){
    char* response = NULL; //must be freed
	String str = "";
	str = this->_serialPort->readStringUntil(terminator);
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

void SerialIface::setSerialTimeout(int timeout){
	this->_serialTimeout = timeout;
}

void SerialIface::setSerialPort(Stream& serialPort){
	this->_serialPort = &serialPort;
}

void SerialIface::setSerialPort(){
	//default implementation for Arancino boards
	#if defined (SERIAL_PORT) && defined(BAUDRATE) && defined (TIMEOUT)
	SERIAL_PORT.begin(BAUDRATE);
	SERIAL_PORT.setTimeout(_serialTimeout);
	this->_serialTimeout = TIMEOUT;
	this->_serialPort = &SERIAL_PORT;
	#endif
}

/******** MQTT interface *********/

//Static variables definition
char* MqttIface::_inputBuffer;
bool MqttIface::_newIncomingMessage = false;
char* MqttIface::_inputTopic;
char* MqttIface::_outputTopic;
char* MqttIface::_serviceTopic;

void MqttIface::ifaceBegin(){
	setClient(*_client);
	setServer(_broker, _port);
	setCallback(_arancinoCallback);

	//+1 because of \n
	_inputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(_daemonID) + strlen(Arancino.id) + strlen("/rsp_to_mcu") + 2, sizeof(char));
	_outputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(_daemonID) + strlen(Arancino.id) + strlen("/cmd_from_mcu") + 2, sizeof(char));
	_serviceTopic = (char*)Arancino.calloc(strlen("arancino/service") + strlen(_daemonID) + strlen(Arancino.id) + 2, sizeof(char));

	strcpy(_inputTopic, "arancino/cortex/");
	strcat(_inputTopic, _daemonID);
	strcat(_inputTopic, "/");
	strcat(_inputTopic, Arancino.id);

	strcpy(_outputTopic, _inputTopic);	//just a quick shortcut

	strcat(_inputTopic, "/rsp_to_mcu");
	strcat(_outputTopic, "/cmd_from_mcu");

	strcpy(_serviceTopic, "arancino/service/");
	strcat(_serviceTopic, _daemonID);

	this->_reconnect();

	this->subscribe(_serviceTopic); //arancino/service/dID
	this->subscribe(_inputTopic);

	//strcat(_serviceTopic, "/");
	//strcat(_serviceTopic, Arancino.id);

	//this->subscribe(_serviceTopic); //arancino/service/dID/cID

	char* discoverytopic = (char*)Arancino.calloc(strlen("arancino/discovery/") + strlen(_daemonID)+1, sizeof(char));
	strcpy(discoverytopic, "arancino/discovery/");
	strcat(discoverytopic, _daemonID);
	this->publish(discoverytopic, Arancino.id);
	Arancino.free(discoverytopic);
}

void MqttIface::sendArancinoCommand(char* command){
	int counter = 0;
	while (counter < MQTT_MAX_RETRIES){
		if (this->publish(_outputTopic, command)){
			return;
		}
		Arancino.println("Failed to send message, retrying.");
		counter++;
	}
	Arancino.println("Failed to send message. Attempting to reconnect");

	//This should not happen. Check if still connected
	this->_reconnect();
}

char* MqttIface::receiveArancinoResponse(char terminator){
	int counter = 0;
	while(!_newIncomingMessage){
		if (counter < MQTT_MAX_RETRIES){
			this->loop();
			counter++;
		} else {
			//No need for cleanup: no message was received nor memory allocated for it
			return NULL;
		}
	}

	//Clean this before going out. _inputBuffer will be freed by caller function
	_newIncomingMessage = false;
	return _inputBuffer;
}

void MqttIface::_arancinoCallback(char* topic, byte* payload, unsigned int length){

	if (!strcmp(topic, _serviceTopic)){
		//should check whether the message is "reset" or no other messages will be sent here?
		Arancino.systemReset();
	} else if (!strcmp(topic, _inputTopic)){
		_inputBuffer = (char*)Arancino.calloc(length+2, sizeof(char));
		memcpy(_inputBuffer, payload, length);
		_inputBuffer[length] = END_TX_CHAR;
		_inputBuffer[length+1] = '\0';
		_newIncomingMessage = true;
	} 
}


void MqttIface::setNetworkClient(Client& networkClient){
	this->_client = &networkClient;
}

void MqttIface::setUsername(char* username){
	this->_username = username;
}

void MqttIface::setPassword(char* password){
	this->_password = password;
}

void MqttIface::setDaemonID(char* daemonID){
	this->_daemonID = daemonID;
}

void MqttIface::setBrokerAddress(char* broker){
	this->_broker = broker;
}

void MqttIface::setPort(int port){
	//basic check for valid port
	if (port>0 && port<65353)
		this->_port = port;
}

void MqttIface::_reconnect(){
	while (!this->connected()){
		if (!this->connect(Arancino.id, _username, _password)){
			//If debug is enabled, tell the user that connection failed
			Arancino.print("Connection failed, retrying in 2 seconds. RC=");
			Arancino.println(this->state());
			Arancino.delay(2000);
		}
	}
}

/******** Bluetooth interface *********/

void BluetoothIface::setBLESerial(Stream& bleUart){
	this->_bleSerial = &bleUart;
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
