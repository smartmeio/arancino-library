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

- void set( String key, String value )
- void set( String key, int value )
- void set( String key, float value )

Parameters
- key: the key name
- value: the value for the specified key. can be String, int o float
*/


#include <Arancino.h>

void setup() {
  Arancino.begin();

  //sets the value 'bar' into the 'foo' key
  Arancino.set("ex01_foo","bar");
}

void loop() {
  //do something
}
