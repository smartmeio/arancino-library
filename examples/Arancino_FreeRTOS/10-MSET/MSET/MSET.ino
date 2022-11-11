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
Sets the given `keys` to their respective `values` MSET replaces existing values with new values, just as regular SET.


- ArancinoPacket mset(char** keys, char**; values, uint len)

Parameters:
- key: the key name
- value: the value for the specified key. can be char*, int, long or float
- isPersistent: optional boolean value to specify if value must be stored
    persistently or not. Default is `false`.

- keys: keys to set;
* values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of `keys` array.


Return value - ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: VOID;
- response: NULL;
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "13.1 - MSet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

char* keys[] = {"EX_13_1_foo1", "EX_13_1_foo2", "EX_13_1_foo3"};
char* values[] = {"value1", "value2", "value3"};

void setup(){

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  ArancinoPacket apckt = Arancino.mset(keys, values, 3);

  if (apckt.isError == 0)
  {
    Arancino.println("MSET OK");
    Arancino.print("Response code: ");
    Arancino.println(apckt.responseCode);
    Arancino.print("Response type: ");
    Arancino.println(apckt.responseType);
    Arancino.print("Response value: ");
    Arancino.println(apckt.response.integer);
  }
  else
  {
    Arancino.println("MSET ERROR");
  }

  Arancino.free(apckt);

  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();

}

void loop() {
  //empty
}

void loopTask(void *pvParameters){
  while(1){
    //do something
  }
}