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
Removes the specified field from the hash stored at key. If field is specified and it does not exist within this hash, this command returns 0. If the key does not exist, it is treated as an empty hash and this command returns 0.

ArancinoPacket hdelPacket(char* key, char* field )

Parameters:
  - key: the name of the key stored in the hash.
  - field: the name of the field stored in the hash at key to delete.
  
Return value
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value.
  - responseType: INT;
  - response.integer: 1 if the field is removed from hash or 0 if the field or the key does not exist in the hash.
*/
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");
  ArancinoPacket temp = Arancino.hdelPacket("foo","bar");
  if (!temp.isError)
  {
    Serial.println("HDEL OK");
    Serial.print("Response code: ");
    Serial.println(temp.responseCode);
    Serial.print("Response type: ");
    Serial.println(temp.responseType);  
    int value = temp.response.integer;
    Serial.println(value ? "Field removed" : "Field/key not found");
  }
  else
  {
    Serial.println("HDEL ERROR");    
  }
}

void loop() {
  //do something
}
