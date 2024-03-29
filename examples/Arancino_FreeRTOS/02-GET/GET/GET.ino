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
Get the value of key. If the key does not exist, the special value NULL is returned.

-char* get( char* key )

Parameters
- key: the name of the key from which the value is retrieved

Return value - String reply:
- the value of the passed key.
- NULL if the key doesn't exist.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "2.1 - Get Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t customLoopTaskHandle;
void customLoopTask(void *pvParameters);

void setup() {

  //Please remember to provide a serial port when not using an Arancino board
  //iface.setSerialPort(Serial);
  iface.setSerialPort();
  Arancino.attachInterface(iface);

  //Debug options
  //Likewise to interface, a Serial port should be provided as argument when using non-Arancino boards
  Arancino.enableDebugMessages();

  Arancino.begin(amdata);
  
	xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){
    //sets the value 'bar' into the 'foo' key
    Arancino.set("EX_02_1","bar");

    //gets the value from the 'foo' key
    char* value = Arancino.get("EX_02_1");
    Arancino.print("EX_02_1 -> ");
    Arancino.println(value);
    //foo -> bar
    Arancino.free(value); //frees memory

    vTaskDelay(2000); //wait 2 seconds

    Arancino.set("EX_02_1","baz");

    //gets the value from the 'foo' key
    value = Arancino.get("EX_02_1");
    Arancino.print("EX_02_1 -> ");
    Arancino.println(value);
    //foo -> baz
    Arancino.free(value); //frees memory

    vTaskDelay(2000); //wait 2 seconds
  }
}
