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
Posts a message to the given channel.

ArancinoPacket publishPacket(char* channel, char* message)
ArancinoPacket publishPacket(int channel, char* message)

Parameters:
  - channel: the name of the channel where the message will be sent.
  - message: message to send.

Return value
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value.
  - responseType: INT;
  - response.integer: the number of clients that received the message.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "12.2 - Publish Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {

  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);

}

void loop(){

    ArancinoPacket apckt = Arancino.publish<ArancinoPacket>("EX_12_2","Hello from Arancino");
    if (!apckt.isError){
       SERIAL_DEBUG.println("PUBLISH OK");
       SERIAL_DEBUG.print("Response code: ");
       SERIAL_DEBUG.println(apckt.responseCode);
       SERIAL_DEBUG.print("Response type: ");
       SERIAL_DEBUG.println(apckt.responseType);
       int resp = apckt.response.integer;
       SERIAL_DEBUG.print("Message sent to ");
       SERIAL_DEBUG.print(resp);
       SERIAL_DEBUG.println(" clients");
       //Message sent to 0 client
     }
     else{
       SERIAL_DEBUG.println("PUBLISH ERROR");
     }

    Arancino.free(apckt);

    delay(5000); //wait 5 seconds

}
