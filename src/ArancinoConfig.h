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


#include <ArancinoDefinitions.h>

#define ARANCINOMQTT
#if defined(ARANCINOMQTT) && !defined(PUBSUBCLIENT_H_)
#define PUBSUBCLIENT_H_
#include <PubSubClient.h>
#endif


class MqttConfig{
	public:
	MqttConfig(Client& client, IPAddress broker, int port, char* name)
		: client(client), broker(broker), port(port), uniqueName(name)
	{}

	Client& client;
	IPAddress broker;
	int port;
	char* uniqueName;
};

class ArancinoConfig{
	public:
		bool _USEID = false;
		int _TIMEOUT = TIMEOUT;
		bool USE_PORT_ID_PREFIX_KEY = false;
		int SERIAL_TIMEOUT = 100;		//timeout 100 ms
		int DECIMAL_DIGITS = 4;			//4 decimal digits (float and double type)
		MqttConfig* mqttConfig;
};