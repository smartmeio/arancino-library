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
Returns all the keys matching the pattern.

ArancinoPacket keysPacket(char* pattern)

Supported glob-style patterns:

h?llo matches hello, hallo and hxllo
h*llo matches hllo and heeeello
h[ae]llo matches hello and hallo, but not hillo
h[^e]llo matches hallo, hbllo, ... but not hello
h[a-b]llo matches hallo and hbllo
Use \ to escape special characters if you want to match them verbatim.

Parameters:
  - pattern: the pattern used to find matching keys.

Return value:
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING_ARRAY;
  - response.stringArray: char** pointer to the string array of keys matching pattern.
*/
#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "04.2 - Keys w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup() {

  Serial.begin(115200);
  Arancino.begin(amdata);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 0, &loopTaskHandle);
  Arancino.set("EX_04_2_pressure", 1023);
  Arancino.set("EX_04_2_humidity", 67.5);
  Arancino.set("EX_04_2_temperature", 24.4);

  Arancino.startScheduler();

}

void loop(){
  //empty
}

void loopTask(void *pvParameters){
  while(1){

    ArancinoPacket apckt = Arancino.keys<ArancinoPacket>();
    char** keys = apckt.response.stringArray;

    if (apckt.isError == 0){

      Serial.println("KEYS OK");
      Serial.print("Response code: ");
      Serial.println(apckt.responseCode);
      Serial.print("Response type: ");
      Serial.println(apckt.responseType);
      for (int i = 0; i < Arancino.getArraySize(keys); i++) {
        Serial.println(keys[i]);
      }
      //pressure
      //humidity
      //temperature
    }
    else{
      Serial.println("KEYS ERROR");
    }

    Arancino.free(keys); //delete the array from memory
    Arancino.free(apckt);

    vTaskDelay(1000); //wait 1 seconds

    apckt = Arancino.keys<ArancinoPacket>("EX_04_2_temp*");
    keys = apckt.response.stringArray;

    if (apckt.isError == 0){

      Serial.println("KEYS OK");
      Serial.print("Response code: ");
      Serial.println(apckt.responseCode);
      Serial.print("Response type: ");
      Serial.println(apckt.responseType);
      for (int i = 0; i < Arancino.getArraySize(keys); i++) {
        Serial.println(keys[i]);
      }
      // temperature

    }
    else{
      Serial.println("KEYS ERROR");
    }

    Arancino.free(keys); //delete the array from memory
    Arancino.free(apckt);
    vTaskDelay(1000); //wait 1 seconds
  }
}
