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
Append new samples to a list of series.


- ArancinoPacket mstore(char** keys, char** samples, uint len)

Parameters:
- keys: keys to set
- samples: corresponding values of the samples
- len: number of keys to set, namely the length of `keys` array.

Return value:
ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: STRING_ARRAY;
- response.stringarray: char** pointer that points to the start of the returned array of insertion timestamps to the series.
*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "17.1 - MStore Packet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t customLoopTaskHandle;
void customLoopTask(void *pvParameters);

char* keys[] = {"EX_sample_foo1", "EX_sample_foo2", "EX_sample_foo3"};

void setup(){

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop() {
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){
    int val1 = random(1,10);
    float val2 = random(150,350)/13.00;
    int val3 = random(20,35);
    char value1[2];
    char value2[10];
    char value3[10];
    itoa(val1,value1,10);
    dtostrf(val2,2,3,value2);
    itoa(val3,value3,10);

    char* samples[]={value1,value2,value3};
    ArancinoPacket apckt = Arancino.mstore<ArancinoPacket>(keys, samples, 3);

    if (!apckt.isError){

      Arancino.println("MSTORE OK");
      Arancino.print("Response code: ");
      Arancino.println(apckt.responseCode);
      Arancino.print("Response type: ");
      Arancino.println(apckt.responseType);

      char** values = apckt.response.stringArray;
      int arraySize = Arancino.getArraySize(values);

      for (int i = 0; i < arraySize; i++){
        Arancino.print(" timestamp ");
        Arancino.print(keys[i]);
        Arancino.print(" -> ");
        Arancino.println(values[0]);
      }
    }else{
      Arancino.println("MSTORE ERROR");
    }

    Arancino.free(apckt);
    vTaskDelay(5000);
  }
}
