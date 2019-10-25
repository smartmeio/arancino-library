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

void setup() {
  Serial.begin(115200);
  Arancino.begin();

  Arancino.set("pressure",1023);
  Arancino.set("humidity",67.5);
  Arancino.set("temperature",24.4);

}

void loop() {

  ArancinoPacket apckt = Arancino.keysPacket();
  char** keys = apckt.response.stringArray;
  if (apckt.isError == 0)
  {
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
    Arancino.free(keys); //delete the array from memory
  }
  else
  {
    Serial.println("KEYS ERROR");    
  }

  delay(1000); //wait 1 seconds

  apckt = Arancino.keysPacket("temp*");
  keys = apckt.response.stringArray;
  if (apckt.isError == 0)
  {
    Serial.println("KEYS OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    for (int i = 0; i < Arancino.getArraySize(keys); i++) {
      Serial.println(keys[i]);
    }
    //temperature
    Arancino.free(keys); //delete the array from memory
  }
  else
  {
    Serial.println("KEYS ERROR");    
  }

  delay(1000); //wait 1 seconds
}
