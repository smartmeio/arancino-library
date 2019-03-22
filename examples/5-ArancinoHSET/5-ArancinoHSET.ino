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
Sets field in the hash stored at key with the specified value. If key does not exist, a new key holding a hash is created. 
If field already exists in the hash, it is overwritten.

- int hset( String key, String field , String value )
- int hset( String key, String field , int value )
- int hset( String key, String field , float value )

Parameters

- key: the name of the key used to create the hash.
- field: the name of the field to store in the hash.
- value: the value to store in the hash with the specified field.

Return value - int reply:
- 1 if field is a new field in the hash and value was set.
- 0 if field already exists in the hash and the value was updated.
*/

#include <Arancino.h>

void setup() {
  Arancino.begin();

  int resp = Arancino.hset("foo","bar","yeah");
  //1
  resp = Arancino.hset("foo","baz","whoo");
  //0
}

void loop() {
  //do something
}

