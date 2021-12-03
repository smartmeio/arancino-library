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
Returns all fields and values of the hash stored at key. In the returned value, every field name is followed by its value.

ArancinoPacket hgetallPacket(char* key )

Parameters:
  - key: the name of the key which holds the hash.

Return value
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING_ARRAY;
  - response.stringArray: char** pointer that can contain the string array of field and value matching key
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "07.2 - HGetAll w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {

  SERIAL_DEBUG.begin(115200);
  Arancino.begin(amdata);
  Arancino.hset("EX_07_02_foo", "bar", "yeah");
  Arancino.hset("EX_07_02_foo", "baz", "whoo");

}

void loop(){

    ArancinoPacket apckt = Arancino.hgetall<ArancinoPacket>("EX_07_02_foo");

    if (!apckt.isError){

      SERIAL_DEBUG.println("HGETALL OK");
      SERIAL_DEBUG.print("Response code: ");
      SERIAL_DEBUG.println(apckt.responseCode);
      SERIAL_DEBUG.print("Response type: ");
      SERIAL_DEBUG.println(apckt.responseType);

      char** values = apckt.response.stringArray;
      int arraySize = Arancino.getArraySize(values);

      for (int i = 0; i < arraySize; i += 2){
        SERIAL_DEBUG.print("foo ");
        SERIAL_DEBUG.print(values[i]);
        SERIAL_DEBUG.print(" = ");
        SERIAL_DEBUG.println(values[i + 1]);
      }
    }
    else{
      SERIAL_DEBUG.print("HGETALL ERROR");
    }

    Arancino.free(apckt); //delete the array from memory

    delay(1000); //wait 1 seconds

}
