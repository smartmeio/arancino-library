/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2019 SmartMe.IO

  Authors:  Andrea Cannistra <andrea@smartme.io>

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

/*
  Returns all the keys matching the pattern.
  Supported glob-style patterns:

  h?llo matches hello, hallo and hxllo
  h*llo matches hllo and heeeello
  h[ae]llo matches hello and hallo, but not hillo
  h[^e]llo matches hallo, hbllo, ... but not hello
  h[a-b]llo matches hallo and hbllo

  Use \ to escape special characters if you want to match them verbatim.

  - char** keys(String pattern)

  Parameters
  - pattern: the pattern used to find matching keys.


  Return value - char** reply:
  - list of keys matching pattern.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "04.1 - Keys Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup() {

  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);
	xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);

  Arancino.set("EX_04_1_pressure", 1023);
  Arancino.set("EX_04_1_humidity", 67.5);
  Arancino.set("EX_04_1_temperature", 24.4);

  Arancino.startScheduler();
}

void loop(){
  //empty
}

void loopTask(void *pvParameters){
  while(1){

    char** keys = Arancino.keys();
    for (int i = 0; i < Arancino.getArraySize(keys); i++) {
      SERIAL_DEBUG.println(keys[i]);
    }
    //pressure
    //humidity
    //temperature
    Arancino.free(keys); //delete the array from memory

    vTaskDelay(1000); //wait 1 seconds

    keys = Arancino.keys("EX_04_1_temp*");  //return all the keys that contains "temp" pattern
    for (int i = 0; i < Arancino.getArraySize(keys) ; i++) {
      SERIAL_DEBUG.println(keys[i]);      //temperature
    }

    Arancino.free(keys); //delete the array from memory

    vTaskDelay(1000); //wait 1 seconds
  }
}
