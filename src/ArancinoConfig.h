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

#ifndef ARANCINOCONFIG_H_
#define ARANCINOCONFIG_H_

#include <ArancinoDefinitions.h>
#include <Arduino.h>

#define ARANCINO_MQTT_IFACE		//debug

#if defined ARANCINO_MQTT_IFACE
#include <PubSubClient.h>
#endif

class ArancinoIface{
	public:
	virtual void ifaceBegin() = 0;
	virtual void sendArancinoCommand(char* command) = 0;
	virtual char* receiveArancinoResponse(char terminator) = 0;
};

/******** INTERFACES *********/

#if defined(ARANCINO_SERIAL_IFACE)

class SerialIface : public ArancinoIface {
	public:
	/*
		Is a time period which a sent command can waits for the response.
		It's expressed in milliseconds.
	*/
	int SERIAL_TIMEOUT = TIMEOUT;

	private:
	void ifaceBegin();
	void sendArancinoCommand(char* command);
	char* receiveArancinoResponse(char terminator);

	bool comm_timeout = false;
};

#elif defined(ARANCINO_MQTT_IFACE)

class MqttIface : public ArancinoIface, public PubSubClient {
	public:
	Client* client;	//Network client
	char* username=NULL;
	char* password=NULL;
	char* daemonID;
	char* broker;	//IP addresses can be passed as well hostnames as strings
	int port=1883;

	private:
	void ifaceBegin();
	void sendArancinoCommand(char* command);
	char* receiveArancinoResponse(char terminator);

	//Since callback function needs to be declared as static, every related variable needs to be static as well
	//not that it matters anyway, no more than one interface will exist at a time so this should be fine
	static char* _inputTopic;
	static char* _outputTopic;
	static char* _serviceTopic;
	static bool _newIncomingMessage;
	static char* _inputBuffer;
	static void _arancinoCallback(char* topic, byte* payload, unsigned int lenght);

};

#elif defined(ARANCINO_BLUETOOTH_IFACE)

class BluetoothIface : public ArancinoIface {
	public:

	private:
	void ifaceBegin();
	void sendArancinoCommand(char* command);
	char* receiveArancinoResponse(char terminator);
	
};

#endif


/******** CONFIGURATIONS *********/

class ArancinoConfig{
	public:
		/*
			It is intended as the precision number of decimal digits for float and
				double data types.
		*/
		int DECIMAL_DIGITS = 4;



		/*
			Attach the Port ID (the ID of the microcontroller) as a prefix of a key name.
				This is useful when you have multiple Ports connectected using the same
				key name.
		*/
		bool USE_PORT_ID_PREFIX_KEY = false;


		/*
			Enables by default the Arduino Loop function as a FreeRTOS Task.
		*/
		bool FREERTOS_LOOP_TASK_ENABLE = false;


		/*
			Sets the default stack size available for the Arduino Loop function when it's managed
				as a FreeRTOS Task. 256 Bytes by default.
		*/
		int FREERTOS_LOOP_TASK_STACK_SIZE = 256;


		/*
			Assignes a priority of the Arduino Loop function when it's managed as a FreeRTOS Task.
				0 -> Lowest Priority.
				9 -> Highest Priority.
		*/
		int FREERTOS_LOOP_TASK_PRIORITY = 0;


		/*
			By providing one of this defines a different interface for library protocol can be selected
			Keep in mind that the interface object may require further configuration inside your sketch 
		*/
		#if defined(ARANCINO_SERIAL_IFACE)
			SerialIface iface;
		#elif defined(ARANCINO_MQTT_IFACE)
			MqttIface iface;
		#elif defined(ARANCINO_BLUETOOTH_IFACE)
			BluetoothIface iface;
		#else
			#error Arancino Interface not selected, please provide one or check the documentation for further details
		#endif 
};

#endif /* ARANCINOCONFIG_H_ */