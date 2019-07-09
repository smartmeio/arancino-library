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
Removes the specified field from the hash stored at key. 
If field is specified and it does not exist within this hash, this command returns 0. 
If the key does not exist, it is treated as an empty hash and this command returns 0.

- char* hdel( char* key, char* field )

Parameters

- key: the name of the key stored in the hash.
- field: the name of the field stored in the hash at key to delete.

Return value - int reply:
- 1 if the field is removed.
- 0 if the field or the key does not exist.
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");
  int value = Arancino.hdel("foo","bar"); //return 1
  char* str = Arancino.hget("foo","bar"); //return NULL
  Serial.print("hget: ");
  Serial.println(str);
  free(str);

}

void loop() {
  //do something
}
