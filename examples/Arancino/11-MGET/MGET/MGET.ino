/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2020 SmartMe.IO

Authors:  Alessio Cosenza <alessio@smartme.io>

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
Returns the `values` of all specified `keys`

- ArancinoPacket mset(char** keys, char**; values, uint len)

- char** mget(char** keys, char** values, uint len)
- ArancinoPacket mget<ArancinoPacket>(char** keys, uint len)

Parameters:
* `keys`: array containing keys to retrieve;
* `len`: number of keys to retrieve.

- keys: keys to set;
* values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of `keys` array.

Return value - chars
In this case an array of `char*` in returned. Every element contains
the value stored in the corresponding key in the `keys` parameter.
If a key doesn't exist the corresponding element in the returned array is `NULL`.

*/

#include <Arancino.h>

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "14.1 - MGet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

char* keys[] = {"EX_14_1_foo1", "EX_14_1_foo2", "EX_14_1_foo3"};

void setup() {

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  Arancino.set("EX_14_1_foo1", "a");
  Arancino.set("EX_14_1_foo3", "c");

}

void loop(){

    char** result = Arancino.mget(keys, 3);

    for(int i = 0; i < Arancino.getArraySize(result); i++) {
      Arancino.print(keys[i]);
      Arancino.print(" -> ");
      Arancino.println(result[i]);
    }

    Arancino.free(result);
    delay(5000);

}
