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

Returns all field names in the hash stored at key.

- String* hkeys( String key )

Parameters
- key: the name of the key which holds the hash.

Return value - *String reply:
- ~list~ of field matching key.

*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  int resp = Arancino.hset("foo","bar","yeah");
  resp = Arancino.hset("foo","baz","whoo");
  delay(5000);
  String* fields = Arancino.hkeys("foo");
  for(int i=0;i<Arancino.getArraySize();i++){
    Serial.print("foo -> ");
    Serial.println(fields[i]);
    // foo -> bar
    // foo -> baz
  }
}

void loop() {
  //do something
}

