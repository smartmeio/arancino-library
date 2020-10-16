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
Returns the value associated with field in the hash stored at key.

ArancinoPacket hgetPacket(char* key, char* field )

Parameters:
  - key: the name of the key which hold the hash.
  - field: the name of the field from which the value is retrieved
  
Return value:
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING;
  - response.string: char* pointer that can contain the value if a value is stored in field at key or NULL if there isn't a value stored.
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  Arancino.hset("ex06p_foo","bar","yeah");
  Arancino.hset("ex06p_foo","baz","whoo");

}

void loop() {

  char* value = Arancino.hget("ex06p_foo","bar");
  Serial.print("foo bar -> ");
  Serial.println(value);
  //foo bar -> yeah
  Arancino.free(value);

  ArancinoPacket apckt = Arancino.hget<ArancinoPacket>("ex06p_foo", "baz");

  if (apckt.isError == 0)
  {
    Serial.println("HGET OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    Serial.print("foo baz -> ");
    Serial.println(apckt.response.string);
  }
  else
  {
    Serial.println("HGET ERROR");
  }

  Arancino.free(apckt);

  delay(5000); //wait 5 seconds
}
