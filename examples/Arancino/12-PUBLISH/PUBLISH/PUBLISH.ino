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
- the number of clients that have received the message.

- void flush()

Return value - void reply
*/

#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "12.1 - Publish Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup() {

  Serial.begin(115200);

  Arancino.begin(amdata);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 0, &loopTaskHandle);

  //int
  char* channel2 = "EX_12_1_channel2";
  int message2 = 123;
  //publish the value 123 into the 'EX_12_1_channel2' channel
  Arancino.publish(channel2, message2);

  //double
  char* channel3 = "EX_12_1_channel3";
  double message3 = 123.456;
  //publish the value 123.456 into the 'EX_12_1_channel3' channel
  Arancino.publish(channel3, message3);
  Arancino.startScheduler();

}

void loop(){
  //empty
}

void loopTask(void *pvParameters) {
  while(1){
    char* channel1 = "EX_12_1_channel1";
    char* message1 = "hooray";
    //publish the value 'hooray' into the 'EX_12_1_channel1' channel
    int num_client = Arancino.publish(channel1, message1);

    Serial.print("message received by ");
    Serial.print(num_client);
    Serial.println(" clients");
    vTaskDelay(10000);
  }
}
