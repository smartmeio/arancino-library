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

- char** hkeys( char* key )

Parameters
- key: the name of the key which holds the hash.

Return value - char** reply:
- list of fields matching key.

*/

#include <Arancino.h>
#define scr_mode 0

ArancinoMetadata amdata = {
  .fwname = "08.1 - HKeys Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000" 
};

void setup() {
  ArancinoConfig acfg;
  Arancino.begin(amdata, acfg, scr_mode);
  Serial.begin(115200);
  
  Arancino.hset("EX_08_1_foo","bar","yeah");
  Arancino.hset("EX_08_1_foo","baz","whoo");

}

void loop() {

  char** fields = Arancino.hkeys("EX_08_1_foo");

  int arraySize;

  if (scr_mode == 0)
    arraySize = Arancino.getArraySize(fields);
  else arraySize = Arancino.getArraySize(fields) - 1;

  for (int i = 0; i < arraySize; i++) {
    Serial.print("EX_08_1_foo -> ");
    Serial.println(fields[i]);
    // foo -> bar
    // foo -> baz
  }

  Arancino.free(fields);

  delay(5000); //wait 5 seconds
}
