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

#include <Arancino.h>
#include <ArancinoDefinitions.h>

class ArancinoTasks{
	public:
		static void deviceIdentification(void *pvPramaters);
        static void interoception(void *pvPramaters);
		static void sendHeartbeat(void *pvPramaters);
	private:
		float mcuTemp();
		bool temp_initialized = false;  //temp inizialized
};

