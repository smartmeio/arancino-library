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
Sets the given `keys` to their respective `values` MSET replaces existing values with new values, just as regular SET.


- ArancinoPacket mset(char** keys, char**; values, uint len)

Parameters:
- key: the key name
- value: the value for the specified key. can be char*, int, long or float
- isPersistent: optional boolean value to specify if value must be stored 
    persistently or not. Default is `false`.

- keys: keys to set;
* values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of `keys` array.


Return value - ArancinoPacket reply: ArancinoPacket containing:
- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: VOID;
- response: NULL;
*/

#include <Arancino.h>
#define scr_mode 0

ArancinoMetadata amdata = {
  .fwname = "13.1 - MSet Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000" 
};

char* keys[] = {"EX_13_1_foo1", "EX_13_1_foo2", "EX_13_1_foo3"};
char* values[] = {"value1", "value2", "value3"};

void setup(){
  ArancinoConfig acfg;
  Arancino.begin(amdata, acfg, scr_mode);
  Serial.begin(115200);

  ArancinoPacket apckt = Arancino.mset(keys, values, 3);

  if (apckt.isError == 0)
  {
    Serial.println("MSET OK");
    Serial.print("Response code: ");
    Serial.println(apckt.responseCode);
    Serial.print("Response type: ");
    Serial.println(apckt.responseType);
    //Serial.print("Response value: ");
    //Serial.println(apckt.response.integer);
  }
  else
  {
    Serial.println("MSET ERROR");
  }

  Arancino.free(apckt);
  
}

void loop() {
  // Do something
}