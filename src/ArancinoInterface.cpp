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
#include <StreamUtils.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CUSTOM_SERIAL)
HardwareSerial SERIAL_PORT(UARTx);
#endif

/*
    All the interface specific methods should be placed here
*/

/******** Serial interface *********/

void SerialIface::ifaceBegin(){
    //Nothing to initialize here.
}

int SerialIface::serialWrite(const char* str, uint32_t len, unsigned long timeout){
	if (!str || !len)
		return 0;

	int sent = 0; //successfully sent chars count
	int written = -1;
	uint32_t i; //sending command index
	unsigned long str_send_ts = millis(); //start timestamp of string sending
	unsigned long char_send_ts; //start timestamp of single char sending
	const unsigned long char_timeout = 100;

	for (i = 0; i < len; i++) {
		written = -1;
		char_send_ts = millis(); 
		do
		{
			written = (*_serialPort).write(str[i]);
			if (written <= 0)
			{
				#if DEBUG
				SERIAL_DEBUG.print("Retry: ");
				SERIAL_DEBUG.print(" 0x");
				SERIAL_DEBUG.print(str[i], HEX);
				SERIAL_DEBUG.print(" = ");
				SERIAL_DEBUG.println(str[i]);
				#endif
			}
		} while (written < 1 && millis() < (char_send_ts + char_timeout));

		if (written < 1)
		{
			#if DEBUG
			SERIAL_DEBUG.print("Error sending: ");
			SERIAL_DEBUG.print(" 0x");
			SERIAL_DEBUG.print(str[i], HEX);
			SERIAL_DEBUG.print(" = ");
			SERIAL_DEBUG.println(str[i]);
			#endif
			break;
		}
		else
		{
			++sent; //increasing successfully sent chars
		}

		if (millis() > (str_send_ts + timeout))
		{
			//Command sending timeout, exit from for-cycle
			#if DEBUG
			SERIAL_DEBUG.println("serialWrite timeout");
			#endif
			break;
		}
	}
	return sent; //if sent < len then error!
}

void SerialIface::sendArancinoCommand(JsonDocument& command){
	size_t docSize;
	char* docStr;

	if (this->comm_timeout){
		while(this->_serialPort->available() > 0){
				this->_serialPort->read();
		}
		comm_timeout=false;

	}
	
	docSize = measureMsgPack(command);
	docStr = (char*)malloc(docSize * sizeof(char));
	serializeMsgPack(command, docStr, docSize);

	uint32_t sent = serialWrite(docStr, docSize, TIMEOUT);
	if (sent < docSize)
	{
		#if DEBUG
		SERIAL_DEBUG.print("Error: sent ");
		SERIAL_DEBUG.print(sent);
		SERIAL_DEBUG.print(" of ");
		SERIAL_DEBUG.println(docSize);
		#endif
	}

	free(docStr);

	#if defined(USEFREERTOS) && defined(USE_TINYUSB)
	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING)
	{
		yield();
	}
	#endif
}

bool SerialIface::receiveArancinoResponse(JsonDocument& response){
	
	DeserializationError error = deserializeMsgPack(response, *_serialPort);
	if (error){
		this->comm_timeout = true;
	}
	return (bool)error;
}


void SerialIface::setSerialPort(Stream& serialPort){
	this->_serialPort = &serialPort;
}

void SerialIface::setSerialPort(){
	//default implementation for Arancino boards
	#if defined(SERIAL_PORT)
	
	#if defined(ARDUINO_ARCH_ESP32) && defined(UART_RX) && defined(UART_TX)
	SERIAL_PORT.begin(BAUDRATE, SERIAL_8N1, UART_RX, UART_TX);
	#else
	SERIAL_PORT.begin(BAUDRATE);
	#endif

	SERIAL_PORT.setTimeout(TIMEOUT);
	this->_serialPort = &SERIAL_PORT;
	#endif
}


/******** MQTT interface *********/

//Static variables definition
bool MqttIface::_newIncomingMessage = false;
char* MqttIface::_inputTopic;
char* MqttIface::_outputTopic;
char* MqttIface::_serviceTopic;
unsigned int MqttIface::_length;
byte* MqttIface::_payload;

void MqttIface::ifaceBegin(){
	setClient(*_client);
	setServer(_broker, _port);
	setCallback(_arancinoCallback);
	setBufferSize(CMD_DOC_SIZE);

	//+1 because of \n
	Arancino.free(_inputTopic);
	Arancino.free(_outputTopic);
	Arancino.free(_serviceTopic);
	_inputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(_daemonID) + strlen(Arancino.id) + strlen("/rsp_to_mcu") + 2, sizeof(char));
	_outputTopic = (char*)Arancino.calloc(strlen("arancino/cortex/") + strlen(_daemonID) + strlen(Arancino.id) + strlen("/cmd_from_mcu") + 2, sizeof(char));
	_serviceTopic = (char*)Arancino.calloc(strlen("arancino/service/") + strlen(_daemonID) + 1, sizeof(char));

	strcpy(_inputTopic, "arancino/cortex/");
	strcat(_inputTopic, _daemonID);
	strcat(_inputTopic, "/");
	strcat(_inputTopic, Arancino.id);

	strcpy(_outputTopic, _inputTopic);	//just a quick shortcut

	strcat(_inputTopic, "/rsp_to_mcu");
	strcat(_outputTopic, "/cmd_from_mcu");

	strcpy(_serviceTopic, "arancino/service/");
	strcat(_serviceTopic, _daemonID);

	if(this->_reconnect()){

		this->subscribe(_serviceTopic); //arancino/service/dID
		this->subscribe(_inputTopic);

		char discoverytopic[(strlen("arancino/discovery/") + strlen(_daemonID)+1)];
		strcpy(discoverytopic, "arancino/discovery/");
		strcat(discoverytopic, _daemonID);
		this->publish(discoverytopic, Arancino.id);
		/*char* discoverytopic = (char*)Arancino.calloc(strlen("arancino/discovery/") + strlen(_daemonID)+1, sizeof(char));
		strcpy(discoverytopic, "arancino/discovery/");
		strcat(discoverytopic, _daemonID);
		this->publish(discoverytopic, Arancino.id);
		Arancino.free(discoverytopic);*/

	}
}

void MqttIface::sendArancinoCommand(JsonDocument& command){
	if (!this->connected()){
		/*Arancino.free(_inputTopic);
		Arancino.free(_outputTopic);
		Arancino.free(_serviceTopic);*/
		this->ifaceBegin();
	}

	if (this->connected()){
		if(this->beginPublish(_outputTopic, measureMsgPack(command),false)){
			/*BufferingPrint bufferedClient(*(this->_client), 64);
			serializeMsgPack(command, bufferedClient);
			bufferedClient.flush();*/
			serializeMsgPack(command, *(this->_client));
			//return;
		}
		this->endPublish();
	}
}

bool MqttIface::receiveArancinoResponse(JsonDocument& response){

	bool error = true;

	unsigned long startMillis = millis();
	do {
		if (!this->connected()){
			/*Arancino.free(_inputTopic);
			Arancino.free(_outputTopic);
			Arancino.free(_serviceTopic);*/
			this->ifaceBegin();
		}
		this->loop();
		delay(10);
	} while(!_newIncomingMessage && (millis() < startMillis + TIMEOUT));


	if (_newIncomingMessage)
	{
		error = (bool)deserializeMsgPack(response, (const char*)this->_payload, this->_length);
		_newIncomingMessage = false;
	}

	return error; // Return error if client is not connected
}

void MqttIface::_arancinoCallback(char* topic, byte* payload, unsigned int length){

	if (!strcmp(topic, _serviceTopic)){
		//should check whether the message is "reset" or no other messages will be sent here?
		Arancino.systemReset();
	} else if (!strcmp(topic, _inputTopic)){
		_length = length;
		_payload = payload;
		_newIncomingMessage = true;
	} 
}


void MqttIface::setNetworkClient(Client& networkClient){
	this->_client = &networkClient;
}

void MqttIface::setUsername(const char* username){
	this->_username = username;
}

void MqttIface::setPassword(const char* password){
	this->_password = password;
}

void MqttIface::setDaemonID(const char* daemonID){
	this->_daemonID = daemonID;
}

void MqttIface::setBrokerAddress(const char* broker){
	this->_broker = broker;
}

void MqttIface::setPort(int port){
	//basic check for valid port
	if (port>0 && port<65353)
		this->_port = port;
}

bool MqttIface::_reconnect(){
	//force disconnect
	this->disconnect();
	char willtopic[(strlen("arancino/service/connection_status/")+strlen(_daemonID)+1+strlen(Arancino.id)+1)];
	strcpy(willtopic, "arancino/service/connection_status/");
	strcat(willtopic, _daemonID);
	strcat(willtopic, "/");
	strcat(willtopic, Arancino.id);
	return this->connect(Arancino.id, _username, _password,willtopic,2,0,"offline");
}

/******** Bluetooth interface *********/


void BluetoothIface::setBLESerial(Stream& bleUart){
	this->_bleSerial = &bleUart;
}

void BluetoothIface::ifaceBegin(){
	#if defined(ARDUINO_ARCH_NRF52)
	//Patch for NRF52 as the used id is the BLE MAC rather than the micro id
	strcpy(Arancino.id, getMacAddr());
	#endif
}

void BluetoothIface::sendArancinoCommand(JsonDocument& command){
	WriteBufferingStream bufferedStream(*_bleSerial, 128);
	serializeMsgPack(command, bufferedStream);
	bufferedStream.flush();
}

bool BluetoothIface::receiveArancinoResponse(JsonDocument& response){
	unsigned long startMillis = millis();
	DeserializationError error;
	do
	{
		error = deserializeMsgPack(response, *_bleSerial);
	}
	while (error != DeserializationError::Ok && millis() < startMillis + TIMEOUT);

	if(error){
		if (++_timeoutCounter == BLUETOOTH_MAX_RETRIES){
			//For each serial timeout this will get incremented - board will reset after a certain number of timeouts
			Arancino.systemReset();
		}
	} else {
		_timeoutCounter = 0;
	}

	return (bool)error;
}