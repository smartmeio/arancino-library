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

#ifndef ARANCINOINTERFACE_H_
#define ARANCINOINTERFACE_H_

#include <Arduino.h>
#include <ArancinoDefinitions.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class ArancinoIface{
	public:
	virtual void ifaceBegin() = 0;
	virtual void sendArancinoCommand(JsonDocument& command) = 0;
	virtual bool receiveArancinoResponse(JsonDocument& response) = 0;
};

/******** INTERFACES *********/

class SerialIface : public ArancinoIface {
	public:
	/*
		Is a time period which a sent command can waits for the response.
		It's expressed in milliseconds.
	*/
	void setSerialPort();
	void setSerialPort(Stream& serialPort);

	private:
	void ifaceBegin();
	int serialWrite(const char* str, uint32_t len, unsigned long timeout);
	void sendArancinoCommand(JsonDocument& command);
	bool receiveArancinoResponse(JsonDocument& response);

	Stream* _serialPort;
	bool comm_timeout = false;
};


class MqttIface : public ArancinoIface, public PubSubClient {
	public:
	void setNetworkClient(Client& networkClient);
	void setUsername(const char* username);
	void setPassword(const char* password);
	void setDaemonID(const char* daemonID);
	void setBrokerAddress(const char* broker);
	void setPort(int port);
	void setTopic(const char *);

	private:
	void ifaceBegin();
	void sendArancinoCommand(JsonDocument& command);
	bool receiveArancinoResponse(JsonDocument& response);

	bool _reconnect();

	const char* _username = NULL;
	const char* _password = NULL;
	const char* _daemonID;
	const char* _topic = "arancino";
	const char* _broker;	//IP addresses can be passed as well hostnames (as strings)
	int _port = 1883;

	Client* _client; //Network client
	//Since callback function needs to be declared as static, every related variable needs to be static as well
	//not that it matters anyway, no more than one interface will exist at a time so this should be fine
	static char* _inputTopic;
	static char* _outputTopic;
	static char* _serviceTopic;
	static bool _newIncomingMessage;
	static unsigned int _length;
	static byte* _payload;
	static void _arancinoCallback(char* topic, byte* payload, unsigned int lenght);
};

class BluetoothIface : public ArancinoIface {
	public:
	void setBLESerial(Stream& bleUart);

	private:
	void ifaceBegin();
	void sendArancinoCommand(JsonDocument& command);
	bool receiveArancinoResponse(JsonDocument& response);
	
	bool comm_timeout = false;
	Stream* _bleSerial;
	unsigned int _timeoutCounter = 0;
};

#endif /* ARANCINOINTERFACE_H_ */