/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2019 SmartMe.IO

Authors:  Sergio Tomasello <sergio@smartme.io>

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
  
  //char*
  char* key1 = "ex01_foo";
  char* value1 = "hooray";
  //sets the value 'hooray' into the 'foo' key
  Arancino.set(key1, value1);

  //int
  char* key2 = "ex01_bar";
  int value2 = 123;
  //sets the value 123 into the 'bar' key
  Arancino.set(key2, value2);

  //double
  char* key3 = "ex01_baz";
  double value3 = 123.456;
  //sets the value 123.456 into the 'baz' key
  Arancino.set(key3, value3);

}

void loop() {
  //do something
}
