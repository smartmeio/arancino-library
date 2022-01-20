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

ArancinoMetadata amdata = {
  .fwname = "14.2 - MGet w/ Packet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

char* keys[] = {"EX_14_2_foo1", "EX_14_2_foo2", "EX_14_2_foo3"};

void setup() {

  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);

  Arancino.set("EX_14_2_foo1", "a");
  Arancino.set("EX_14_2_foo2", "b");
  Arancino.set("EX_14_2_foo3", "c");

  Arancino.startScheduler();

}

void loop(){
  //empty
}

void loopTask(void *pvParameters) {
  while(1){

    ArancinoPacket apckt = Arancino.mget<ArancinoPacket>(keys, 3);

    if (!apckt.isError)
    {
      SERIAL_DEBUG.println("MGET OK");
      SERIAL_DEBUG.print("Response code: ");
      SERIAL_DEBUG.println(apckt.responseCode);
      SERIAL_DEBUG.print("Response type: ");
      SERIAL_DEBUG.println(apckt.responseType);

      for(int i = 0; i < Arancino.getArraySize(apckt.response.stringArray); i++) {
        SERIAL_DEBUG.print(keys[i]);
        SERIAL_DEBUG.print(" -> ");
        SERIAL_DEBUG.println(apckt.response.stringArray[i]);
      }
      Arancino.free(apckt); //delete the string from memory
    }
    else
    {
      SERIAL_DEBUG.println("MGET ERROR");
    }

    vTaskDelay(5000);
  }
}
