/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2019 SmartMe.IO

  Authors:  Dario Gogliandolo

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
Returns the value associated with field in the hash stored at key.

ArancinoPacket hgetPacket(char* key, char* field )

Parameters:
  - key: the name of the key which hold the hash.
  - field: the name of the field from which the value is retrieved

Return value:
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING;
  - response.string: char* pointer that can contain the value if a value is stored in field at key or NULL if there isn't a value stored.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "06.2 - HGet w/ Packet Example",
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

  Arancino.hset("EX_06_2_foo","bar","yeah");
  Arancino.hset("EX_06_2_foo","baz","whoo");

  Arancino.startScheduler();

}

void loop(){
  //empty
}

void loopTask(void *pvParameters){
  while(1){

    ArancinoPacket apckt = Arancino.hget<ArancinoPacket>("EX_06_2_foo", "baz");

    if (apckt.isError == 0)
    {
      SERIAL_DEBUG.println("HGET OK");
      SERIAL_DEBUG.print("Response code: ");
      SERIAL_DEBUG.println(apckt.responseCode);
      SERIAL_DEBUG.print("Response type: ");
      SERIAL_DEBUG.println(apckt.responseType);
      SERIAL_DEBUG.print("foo baz -> ");
      SERIAL_DEBUG.println(apckt.response.string);
    }
    else
    {
      SERIAL_DEBUG.println("HGET ERROR");
    }

    Arancino.free(apckt);

    vTaskDelay(5000); //wait 5 seconds
  }
}
