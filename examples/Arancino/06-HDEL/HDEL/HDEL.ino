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

//Arancino interface
SerialIface iface;

ArancinoMetadata amdata = {
  .fwname = "10.1 - HDel Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {

  iface.setSerialPort();
  Arancino.attachInterface(iface);

  Arancino.enableDebugMessages();
  Arancino.begin(amdata);

  Arancino.hset("EX_10_1_foo","bar","yeah");
  Arancino.hset("EX_10_1_foo","baz","whoo");

  int value = Arancino.hdel("EX_10_1_foo","bar"); //return 1
  char* str = Arancino.hget("EX_10_1_foo","bar"); //return NULL

  Arancino.print("HGet: ");
  Arancino.println(str);

  Arancino.free(str);

}

void loop() {
  //do something
}
