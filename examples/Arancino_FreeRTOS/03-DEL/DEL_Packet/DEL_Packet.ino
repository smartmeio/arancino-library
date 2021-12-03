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
Removes the specified key. A key is ignored if it does not exist.

ArancinoPacket delPacket(char* key )

Parameters:
  - key: the name of the key to delete.
  - Return value

ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: INT;
  - response: The number of keys that were removed.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "03.2 - Del w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000" 
};

void setup() {

  SERIAL_DEBUG.begin(115200);
  Arancino.begin(amdata);

  Arancino.set("EX_03_2_foo","bar");

  ArancinoPacket apckt = Arancino.del<ArancinoPacket>("EX_03_2_baz");
  
  if (apckt.isError == 0){

    SERIAL_DEBUG.println("DEL OK");
    SERIAL_DEBUG.print("Response code: ");
    SERIAL_DEBUG.println(apckt.responseCode);
    SERIAL_DEBUG.print("Response type: ");
    SERIAL_DEBUG.println(apckt.responseType);
    SERIAL_DEBUG.println(apckt.response.integer ? "Key deleted" : "Key not found");
    //0
  }
  else{
    SERIAL_DEBUG.println("DEL ERROR");    
  }
  
  Arancino.free(apckt); //delete packet from memory

  apckt = Arancino.del<ArancinoPacket>("EX_03_2_foo");
  
  if (apckt.isError == 0){
    
    SERIAL_DEBUG.println("DEL OK");
    SERIAL_DEBUG.print("Response code: ");
    SERIAL_DEBUG.println(apckt.responseCode);
    SERIAL_DEBUG.print("Response type: ");
    SERIAL_DEBUG.println(apckt.responseType);
    SERIAL_DEBUG.println(apckt.response.integer ? "Key deleted" : "Key not found");
    //1
  }
  else{
    SERIAL_DEBUG.println("DEL ERROR");    
  }
  
  Arancino.free(apckt); //delete packet from memory
  
  Arancino.startScheduler();

}

void loop(){
  //empty
}