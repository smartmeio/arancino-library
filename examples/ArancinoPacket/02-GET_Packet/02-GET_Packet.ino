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
  Get the value of key. If the key does not exist, the special value NULL is returned.

  -ArancinoPacket getPacket( char* key )

Parameters:
  - key: the name of the key from which the value is retrieved
  - Return value
  
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: STRING;
  - response.string: char* pointer that can contain the value of selected key or NULL if the key doesn't exist.
*/

#include <Arancino.h>

void setup() {
  Arancino.begin();
  Serial.begin(115200);
}

void loop() {

  //sets the value 'bar' into the 'foo' key
  Arancino.set("foo","bar");

  ArancinoPacket temp = Arancino.getPacket("foo");
  if (!temp.isError)
  {
    Serial.println("GET OK");
    Serial.print("Response code: ");
    Serial.println(temp.responseCode);
    Serial.print("Response type: ");
    Serial.println(temp.responseType);

    Serial.print("foo -> ");
    Serial.println(temp.response.string);
    //foo -> bar
    Arancino.free(temp); //delete the string from memory
  }
  else
  {
    Serial.println("GET ERROR");
  }

  delay(2000); //wait 2 seconds
}
