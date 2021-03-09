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


Return value:
ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: STRING_ARRAY;
- response.stringarray: char** pointer that points to the start of the returned array of strings.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "14.2 - MGet w/ Packet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000" 
};

char* keys[] = {"EX_14_2_foo1", "EX_14_2_foo2", "EX_14_2_foo3"};

void setup() {
  
  Arancino.begin(amdata);
  Serial.begin(115200);

  Arancino.set("EX_14_1_foo1", "a");
  Arancino.set("EX_14_1_foo3", "c");


}

void loop() {

ArancinoPacket apckt = Arancino.mget<ArancinoPacket>(keys, 3);

  if (!apckt.isError)
  {
    Serial.println("MGET OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);

    for(int i = 0; i < Arancino.getArraySize(temp.response.stringArray); i++) {
      Serial.print(keys[i]);
      Serial.print(" -> ");
      Serial.println(apckt.response.stringArray[i]);
    }
    Arancino.free(apckt); //delete the string from memory
  }
  else
  {
    Serial.println("MGET ERROR");
  }

  delay(5000);
  
}