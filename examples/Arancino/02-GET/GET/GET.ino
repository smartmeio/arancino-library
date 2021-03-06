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
Get the value of key. If the key does not exist, the special value NULL is returned.

-char* get( char* key )

Parameters
- key: the name of the key from which the value is retrieved

Return value - String reply:
- the value of the passed key.
- NULL if the key doesn't exist.
*/

#include <Arancino.h>

void setup() {
  Arancino.begin();
  Serial.begin(115200);
}

void loop() {

  //sets the value 'bar' into the 'foo' key
  Arancino.set("ex02_foo","bar");

  //gets the value from the 'foo' key
  char* value = Arancino.get("ex02_foo");
  Serial.print("ex02_foo -> ");
  Serial.println(value);
  //foo -> bar
  Arancino.free(value); //frees memory

  delay(2000); //wait 2 seconds

  Arancino.set("ex02_foo","baz");

  //gets the value from the 'foo' key
  value = Arancino.get("ex02_foo");
  Serial.print("ex02_foo -> ");
  Serial.println(value);
  //foo -> baz
  Arancino.free(value); //frees memory
  
  delay(2000); //wait 2 seconds
}
