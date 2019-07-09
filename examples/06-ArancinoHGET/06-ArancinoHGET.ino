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
Returns the value associated with field in the hash stored at key.

- char* hget( char* key, char* field )

Parameters

- key: the name of the key which hold the hash.
- field: the name of the field from which the value is retrieved


Return value - char* reply:
- the value if a value is stored in field at key.
- NULL if there isn't a value stored.
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");

}

void loop() {
  
  char* value = Arancino.hget("foo","bar");
  Serial.print("foo bar -> ");
  Serial.println(value);
  //foo bar -> yeah
  free(value);
  
  value = Arancino.hget("foo","baz");
  Serial.print("foo baz -> ");
  Serial.println(value);
  //foo bar -> whoo
  free(value);

  delay(5000); //wait 5 seconds

}
