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
Returns all field names in the hash stored at key.

ArancinoPacket hkeysPacket( String key )

Parameters:
  - key: the name of the key which holds the hash.
  
Return value
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING_ARRAY;
  - response.stringArray: char** pointer to the string array of fields matching key.
*/
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);

  Arancino.hset("ex08p_foo","bar","yeah");
  Arancino.hset("ex08p_foo","baz","whoo");

}

void loop() {

  ArancinoPacket apckt = Arancino.hkeys<ArancinoPacket>("ex08p_foo");
  char** fields = apckt.response.stringArray;
  if (!apckt.isError)
  {
    Serial.println("HKEYS OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    for (int i = 0; i < Arancino.getArraySize(fields); i++) {
      Serial.print("ex08p_foo -> ");
      Serial.println(fields[i]);
      // foo -> bar
      // foo -> baz
    }
    Arancino.free(fields);
  }
  else
  {
    Serial.println("HKEYS ERROR");
  }
  
  Arancino.free(apckt);
  delay(5000); //wait 5 seconds
}
