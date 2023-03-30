/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2020 SmartMe.IO

Authors:  Alessio Cosenza <alessio@smartme.io>

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
Returns the `values` of all specified `keys`

- ArancinoPacket mset(char** keys, char**; values, uint len)

- char** mget(char** keys, char** values, uint len)
- ArancinoPacket mget<ArancinoPacket>(char** keys, uint len)

Parameters:
* `keys`: array containing keys to retrieve;
* `len`: number of keys to retrieve.

- keys: keys to set;
* values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of `keys` array.


Return value:
ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: STRING_ARRAY;
- response.stringarray: char** pointer that points to the start of the returned array of strings.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "14.2 - MGet w/ Packet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t customLoopTaskHandle;
void customLoopTask(void *pvParameters);

char* keys[] = {"EX_14_2_foo1", "EX_14_2_foo2", "EX_14_2_foo3"};

void setup() {

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  Arancino.set("EX_14_2_foo1", "a");
  Arancino.set("EX_14_2_foo2", "b");
  Arancino.set("EX_14_2_foo3", "c");

  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void customLoopTask(void *pvParameters) {
  while(1){

    ArancinoPacket apckt = Arancino.mget<ArancinoPacket>(keys, 3);

    if (!apckt.isError)
    {
      Arancino.println("MGET OK");
      Arancino.print("Response code: ");
      Arancino.println(apckt.responseCode);
      Arancino.print("Response type: ");
      Arancino.println(apckt.responseType);

      for(int i = 0; i < Arancino.getArraySize(apckt.response.stringArray); i++) {
        Arancino.print(keys[i]);
        Arancino.print(" -> ");
        Arancino.println(apckt.response.stringArray[i]);
      }
      Arancino.free(apckt); //delete the string from memory
    }
    else
    {
      Arancino.println("MGET ERROR");
    }

    vTaskDelay(5000);
  }
}
