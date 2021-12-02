/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2019 SmartMe.IO

  Authors:  Dario Gogliandolo

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

- ArancinoPacket set(char* key, char*; value [, bool isPersistent] )
- ArancinoPacket set(char* key,  int value [, bool isPersistent] )
- ArancinoPacket set(char* key, float value [, bool isPersistent] )
- ArancinoPacket set(char* key, long value, [, bool isPersistent] )
- ArancinoPacket set(char* key, uint32_t value, [, bool isPersistent] )

Parameters:
- key: the key name
- value: the value for the specified key. can be char*, int, long or float
- isPersistent: optional boolean value to specify if value must be stored
    persistently or not. Default is `false`.

Return value - ArancinoPacket reply: ArancinoPacket containing:

- isError: API call outcome (true or false);
- responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
- responseType: VOID;
- response: NULL;
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "01.2 - Set w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {

  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);

  ArancinoPacket apckt = Arancino.set("EX_01_2_foo", "bar");

  if (apckt.isError == 0){

    SERIAL_DEBUG.println("SET OK");
    SERIAL_DEBUG.print("Response code: ");
    SERIAL_DEBUG.println(apckt.responseCode);
    SERIAL_DEBUG.print("Response type: ");
    SERIAL_DEBUG.println(apckt.responseType);
    SERIAL_DEBUG.print("Response value: ");
    SERIAL_DEBUG.println(apckt.response.integer);

  }
  else{
    SERIAL_DEBUG.println("SET ERROR");
  }

  Arancino.free(apckt); //delete packet from memory

}

void loop() {
    //do something
}
