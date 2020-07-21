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
Posts a message to the given channel.

- int publish(String channel, String message);
- int publish(int channel, String message);

Parameters:
- channel: channel where message will be send
- message: message to send

Return value - int reply: 
- the number of clients that received the message.

- void flush()

Return value - void reply
*/

#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);

}

void loop() {
  
  int resp = Arancino.publish("0","hello from Arancino");
  Serial.print("message send to ");
  Serial.print(resp);
  Serial.println(" client");
  //message send to 0 client 
  
  delay(5000); //wait 5 seconds
  
}
