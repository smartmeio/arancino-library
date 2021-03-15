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
Returns all values in the hash stored at key.

ArancinoPacket hvalsPacket(char* key )

Parameters:
  - key: the name of the key which holds the hash.
  
Return value
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING_ARRAY;
  - response.stringArray: char** pointer to the string array of values matching key.
*/
#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "09.2 - HVals w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000" 
};

void setup() {

  Arancino.begin(amdata);
  Serial.begin(115200);

  Arancino.hset("EX_09_2_foo", "bar", "yeah");
  Arancino.hset("EX_09_2_foo", "baz", "whoo");

}

void loop() {
  
  ArancinoPacket apckt = Arancino.hvals<ArancinoPacket>("EX_09_2_foo");
  char** values = apckt.response.stringArray;
  
  if (!apckt.isError){

    Serial.println("HVALS OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    for (int i = 0; i < Arancino.getArraySize(values); i++) {
      Serial.print("EX_09_2_foo -> ");
      Serial.println(values[i]);
      // foo -> yeah
      // foo -> whoo
    }
    Arancino.free(values);
  }
  else{
    Serial.println("HVALS ERROR");
  }  

  Arancino.free(apckt);
  delay(5000); //wait 5 seconds
}
