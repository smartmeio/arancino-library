/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2019 SmartMe.IO

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
Removes the specified key. A key is ignored if it does not exist.

- int del( String key )


Parameters
- key: the name of the key to delete.

Return value - int reply:
The number of keys that were removed.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "03.1 - Del Example",
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

  Arancino.set("EX_03_1_foo","bar");

  int num = Arancino.del("EX_03_1_baz");
  Arancino.println(num ? "Key deleted" : "Key not found"); //0

  num = Arancino.del("EX_03_1_foo");
  Arancino.println(num ? "Key deleted" : "Key not found"); //1
  
  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();
}

void loop() {
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){
    //do something
  }
}