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

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "06.2 - HGet w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t customLoopTaskHandle;
void customLoopTask(void *pvParameters);

void setup() {

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  Arancino.hset("EX_06_2_foo","bar","yeah");
  Arancino.hset("EX_06_2_foo","baz","whoo");

  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){

    ArancinoPacket apckt = Arancino.hget<ArancinoPacket>("EX_06_2_foo", "baz");

    if (apckt.isError == 0)
    {
      Arancino.println("HGET OK");
      Arancino.print("Response code: ");
      Arancino.println(apckt.responseCode);
      Arancino.print("Response type: ");
      Arancino.println(apckt.responseType);
      Arancino.print("foo baz -> ");
      Arancino.println(apckt.response.string);
    }
    else
    {
      Arancino.println("HGET ERROR");
    }

    Arancino.free(apckt);

    vTaskDelay(5000); //wait 5 seconds
  }
}
