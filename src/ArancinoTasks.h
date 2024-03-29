/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2021 SmartMe.IO

Authors:  Andrea Cannistrà <andrea@smartme.io>

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

#ifndef ARANCINOTASKS_H_
#define ARANCINOTASKS_H_

#include <ArancinoDefinitions.h>
#include <Arancino.h>
#include <ArduinoJson.h>

class ArancinoTasks{
	#ifdef USEFREERTOS
	public:
		ArancinoTasks();
		static void serviceTask(void *pvPramaters);
		TimerHandle_t identificationTimer;
		TimerHandle_t interoceptionTimer;
		TimerHandle_t heartbeatTimer;

		static void identificationCallback(TimerHandle_t xTimer);
		static void interoceptionCallback(TimerHandle_t xTimer);
		static void heartbeatCallback(TimerHandle_t xTimer);
		
	private:
		static float mcuTemp();
		static DynamicJsonDocument* cmd_doc;
		static DynamicJsonDocument* rsp_doc;
		static SemaphoreHandle_t jsonMutex;

		static void identificationTask();
		static void interoceptionTask();
		static void heartbeatTask();

		static bool _identificationFlag;
		static bool _interoceptionFlag;
		static bool _heartbeatFlag;
		
	#endif
};

#endif 