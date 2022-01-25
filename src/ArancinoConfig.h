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
};