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

[[deprecated]]
- void begin(ArancinoMetadata amdata, bool useid = false, int timeout = TIMEOUT );

- void begin(ArancinoMetadata amdata, ArancinoConfig aconfig);

Parameters:

- ArancinoConfig: is class composed only by attributes:
  - _TIMEOUT: (see the following paragraph)
  - _USEID: (see the following paragraph)

  The convenience of using class X is to be able to increase the number of parameters
  without necessarily having to change the prototypes.


the use of the following is deprecated and will be remove in the next major release.
- timeout:  Represents the time that each command sent (via API call) will wait
            for a response. Otherwise the delayed command will be skipped. When the
            `begin` function is called w/o `timeout` it is assumed `100ms` as
            default value of timeout.
- useid: Default false. If true each key setted up is prefixed with ID of the microntroller.
        It's useful when you connect multiple microntroller with the same firmware (using the same keys) to
        one Arancino Module; By this way, at the application level, you can distinguish keys by
        microntroller id.

*/


#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "00.1 - Begin Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

void setup() {

  // Arancino Configuration
  ArancinoConfig acfg;
  acfg._TIMEOUT = 200;
  acfg.DECIMAL_DIGITS=4;

  //calls begin w/o paramenter: it is assumed 100ms timeout by default
  Arancino.begin(amdata, acfg);

  //calls begin w/ 200ms timeout trough Arancino Config
  //Arancino.begin(amdata, acfg);
}

void loop() {
    //do something
}
