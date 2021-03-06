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
Set 'key' to hold the string 'value'. If 'key' already holds a 'value', it is overwritten, regardless of its type.

- ArancinoPacket set( char* key, char* value )
- ArancinoPacket set( char* key, int value )
- ArancinoPacket set( char* key, float value )

Parameters:
- key: the key name
- value: the value for the specified key. can be char*, int o float

Return value - ArancinoPacket reply: ArancinoPacket containing:

- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: VOID;
- response: NULL;
*/
#include <Arancino.h>

void setup() {
  Arancino.begin();

  ArancinoPacket apckt = Arancino.set("ex01p_foo", "bar");
  if (apckt.isError == 0)
  {
    Serial.println("SET OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    Serial.print("Response value: ");
    Serial.println(apckt.response.integer);
  }
  else
  {
    Serial.println("SET ERROR");
  }

  Arancino.free(apckt); //delete packet from memory

}

void loop() {
    //do something
}
