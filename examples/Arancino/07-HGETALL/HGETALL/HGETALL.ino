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
  Returns all fields and values of the hash stored at key. In the returned value, every field name is followed by its value.

  - char** hgetall( char* key )

  Parameters
  - key: the name of the key which holds the hash.

  Return value - char** reply:
  - list of fields and values matching key.
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();

  Serial.begin(115200);

  Arancino.hset("ex07_foo", "bar", "yeah");
  Arancino.hset("ex07_foo", "baz", "whoo");

}

void loop() {
  char** values = Arancino.hgetall("ex07_foo");
  int arraySize = Arancino.getArraySize(values);
  for (int i = 0; i < arraySize; i += 2)
  {
    Serial.print("foo ");
    Serial.print(values[i]);
    Serial.print(" = ");
    Serial.println(values[i + 1]);
  }
  Arancino.free(values); //delete the array from memory

  delay(5000); //wait 5 seconds
}
