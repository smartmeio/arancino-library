/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2019 SmartMe.IO

Authors:  Andrea Cannistra <andrea@smartme.io>

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
Sets field in the hash stored at key with the specified value. If key does not exist, a new key holding a hash is created. If field already exists in the hash, it is overwritten.

ArancinoPacket hset(char* key, char* field , char* value )
ArancinoPacket hset(char* key, char* field , int value )
ArancinoPacket hset(char* key, char* field , float value )

Parameters:
  - key: the name of the key used to create the hash.
  - field: the name of the field to store in the hash.
  - value: the value to store in the hash with the specified field.
  
Return value:
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: VOID;
  - response: NULL;
*/


#include <Arancino.h>

void setup() {
  Arancino.begin();

  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","bar","whoo");
}

void loop() {
  //do something
}
