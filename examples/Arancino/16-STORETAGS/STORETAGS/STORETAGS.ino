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

- ArancinoPacket storetags(char* keys, char** tags, char** values, uint len)

Parameters:
- key: key to store;
- tags:  the name of the tags to store;
- values: the value to store with the specified tag;
- len: number of tags.

Return value - void

*/

#include <Arancino.h>
#include <avr/dtostrf.h>

ArancinoMetadata amdata = {
  .fwname = "16.1 - StoreTags Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

char* key = "EX_sample_1";
char* tags[] = {"EX_tags_1", "EX_tags_2", "EX_tags_3"};


void setup() {
  Serial.begin(115200);
  Arancino.begin(amdata);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 0, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop(){
  //empty
}

void loopTask(void *pvParameters) {
  while(1){
    int val1 = random(1,10);
    float val2 = random(150.00,350.00)/13.00;
    int val3 = random(20,35);
    char value1[2];
    char value2[10];
    char value3[10];
    itoa(val1,value1,10);
    dtostrf(val2,2,3,value2);
    itoa(val3,value3,10);

    char* samples[]={value1,value2,value3};
    Arancino.storetags(key, tags, samples, 3);

    vTaskDelay(5000);
  }
}
