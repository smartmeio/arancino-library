/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2020 SmartMe.IO

Authors:  Andrea CAnnistrà <andrea@smartme.io>

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
Append (or create and append) a new sample to the series specified by key.


- ArancinoPacket store(char* key, int value)
- ArancinoPacket store(char* key, float value)
- ArancinoPacket store(char* key, long value)
- ArancinoPacket store(char* key, uint32_t value)

Parameters:
- key: key to store
- value: the value for the specified key. can be int, long or float

Return value - ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: STRING;
- response.string: char* pointer that contain the insertion timestamp to the series.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "15.1 - Store Packet Example",
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

  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void customLoopTask(void *pvParameters) {
  while(1){
    char* key1 = "EX_sample_1";
    int sample1 = random(0,10);
    ArancinoPacket apckt = Arancino.store<ArancinoPacket>(key1, sample1);

    if (apckt.isError == 0)
    {
      Arancino.println("STORE OK");
      Arancino.print("Response code: ");
      Arancino.println(apckt.responseCode);
      Arancino.print("Response type: ");
      Arancino.println(apckt.responseType);
      Arancino.print("foo baz -> ");
      Arancino.println(apckt.response.string);
    }
    else
    {
      Arancino.println("STORE ERROR");
    }

    Arancino.free(apckt);
    vTaskDelay(2000);
  }
}
