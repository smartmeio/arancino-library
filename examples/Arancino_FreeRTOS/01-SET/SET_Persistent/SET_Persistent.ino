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
/*
Set 'key' to hold the string 'value'. If 'key' already holds a 'value', it is overwritten, regardless of its type.

- ArancinoPacket set(char* key, char*; value [, bool isPersistent] )
- ArancinoPacket set(char* key,  int value [, bool isPersistent] )
- ArancinoPacket set(char* key, float value [, bool isPersistent] )
- ArancinoPacket set(char* key, long value, [, bool isPersistent] )
- ArancinoPacket set(char* key, uint32_t value, [, bool isPersistent] )

Parameters:
- key: the key name
- value: the value for the specified key. can be char*, int, long or float
- isPersistent: optional boolean value to specify if value must be stored
    persistently or not. Default is `false`.

Return value - ArancinoPacket reply: ArancinoPacket containing:

- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: VOID;
- response: NULL;
*/

#include <Arancino.h>

//Interface for Arancino protocol
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "01 - Set Persistent and Volatile Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup() {

  iface.setSerialPort();
  Arancino.attachInterface(iface);
  Arancino.begin(amdata);

  Arancino.set("EX_01_3_key_1", "bar", true);      //persistent key
  Arancino.set("EX_01_3_key_2", "bar", 1);         //persistent key
  Arancino.set("EX_01_3_key_3", "bar", false);     //volatile key
  Arancino.set("EX_01_3_key_4", "bar", NULL);      //volatile key
  Arancino.set("EX_01_3_key_5", "bar");            //volatile key

  xTaskCreate(loopTask, "loopTask", 512, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop() {
    //do something
}
