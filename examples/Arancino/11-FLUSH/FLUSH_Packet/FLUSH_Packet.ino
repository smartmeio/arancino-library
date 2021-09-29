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
Delete all the keys.

ArancinoPacket flush()

Return value:
ArancinoPacket reply: ArancinoPacket containing:
  - isError: API call outcome (true or false);
  - responseCode: the response code value. (Response code -> https://git.smartme.io/smartme.io/arancino/arancino-library#variables)
  - responseType: VOID;
  - response: NULL.
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "11.2 - Flush w/ Packet Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
//TaskHandle_t loopTaskHandle;
//void loopTask(void *pvParameters);

void setup() {

  SERIAL_DEBUG.begin(115200);

  Arancino.begin(amdata);

  Arancino.set("EX_11_2_foo","bar");
  Arancino.set("EX_11_2_foo","baz");
  //delete all the keys

  ArancinoPacket apckt = Arancino.flush();

  if (!apckt.isError){
    SERIAL_DEBUG.println("FLUSH OK");
    SERIAL_DEBUG.print("Response code: ");
    SERIAL_DEBUG.println(apckt.responseCode);
    SERIAL_DEBUG.print("Response type: ");
    SERIAL_DEBUG.println(apckt.responseType);
  }
  else{
    SERIAL_DEBUG.println("FLUSH ERROR");
  }

  Arancino.free(apckt);
  Arancino.startScheduler();

}

void loop() {
  //do something
}
