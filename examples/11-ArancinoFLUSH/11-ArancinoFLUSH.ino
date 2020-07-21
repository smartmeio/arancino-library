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
Delete all the keys.

- void flush()

Return value - void reply
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Arancino.set("foo","bar");
  Arancino.set("foo","baz");
  //delete all the keys
  Arancino.flush();

}

void loop() {
  //do something
}
