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

- String hget( String key, String field )

Parameters

- key: the name of the key which hold the hash.
- field: the name of the field from which the value is retrieved


Return value - String reply:
- the value if a value is stored in field at key.
- NULL if there isn't a value stored.
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  int resp = Arancino.hset("ex06_foo","bar","yeah");
  resp = Arancino.hset("ex06_foo","baz","whoo");

}

void loop() {
  
  String value = Arancino.hget("ex06_foo","bar");
  Serial.print("foo bar -> ");
  Serial.println(value);
  //foo bar -> yeah
  
  value = Arancino.hget("ex06_foo","baz");
  Serial.print("foo baz -> ");
  Serial.println(value);
  //foo bar -> whoo

  delay(5000); //wait 5 seconds

}

