/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2020 SmartMe.IO

Authors:  Andrea CAnnistrà <andrea@smartme.io>

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
Append (or create and append) a new sample to the series specified by key.


- ArancinoPacket store(char* key, int value)
- ArancinoPacket store(char* key, float value)
- ArancinoPacket store(char* key, long value)
- ArancinoPacket store(char* key, uint32_t value)

Parameters:
- key: key to store
- value: the value for the specified key. can be int, long or float

Return value - String reply:
- insertion timestamp.
*/


#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "15.1 - Store Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {
  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);

}

void loop(){
    //do something
    char* key1 = "EX_sample_1";
    long sample1 = random(100,400)/36;
    char* timestamp = Arancino.store(key1, sample1);
    SERIAL_DEBUG.print("timestamp -> ");
    SERIAL_DEBUG.println(timestamp);

    Arancino.free(timestamp);

    delay(2000);

}
