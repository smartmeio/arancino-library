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
Start the communication with the Arancino Module.

- void begin(int timeout)
- void begin()

Parameters
- timeout:  represents the time that each command sent (via API call) will wait
            for a response. Otherwise the delayed command will be skipped. When the
            `begin` function is called w/o `timeout` it is assumed `100ms` as
            default value of timeout.
*/


#include <Arancino.h>

void setup() {

  //calls begin w/o paramenter: it is assumed 100ms timeout by default
  Arancino.begin();

  //calls begin w/ 200ms timeout
  //Arancino.begin(200);
}

void loop() {
    //do something
}
