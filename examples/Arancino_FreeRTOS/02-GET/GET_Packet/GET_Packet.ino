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
  Get the value of key. If the key does not exist, the special value NULL is returned.

  -ArancinoPacket getPacket( char* key )

Parameters:
  - key: the name of the key from which the value is retrieved
  - Return value

ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING;
  - response.string: char* pointer that can contain the value of selected key or NULL if the key doesn't exist.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "2.2 - Get w/ Packet Example",
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
  //create a task for loop
  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){
    //sets the value 'bar' into the 'foo' key
    Arancino.set("EX_02_2_foo","bar");

    ArancinoPacket apckt = Arancino.get<ArancinoPacket>("EX_02_2_foo");
    if (!apckt.isError)
    {
      Arancino.println("GET OK");
      Arancino.print("Response code: ");
      Arancino.println(apckt.responseCode);
      Arancino.print("Response type: ");
      Arancino.println(apckt.responseType);

      Arancino.print("EX_02_2_foo -> ");
      Arancino.println(apckt.response.string);
      //foo -> bar

    }
    else
    {
      Arancino.println("GET ERROR");
    }

    Arancino.free(apckt); //delete packet from memory
    vTaskDelay(2000); //wait 2 seconds
  }
}
