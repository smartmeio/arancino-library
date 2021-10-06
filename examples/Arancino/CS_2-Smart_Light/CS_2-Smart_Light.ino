/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2021 SmartMe.IO

Authors:  Marco Calapristi <marco.calapristi@smartme.io>

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
 * CS_2 Smart Light
 * This case scenario shows how to implement a smart light by using an RGB LED
 * and Arancino Library. This sketch periodically fetches RGB values from database
 * and changes LED accordingly. A Redis-cli can be used to change values during runtime
 */

 #include <Arancino.h>

 #define redPin 12
 #define greenPin 9
 #define bluePin 24

//FreeRtos
TaskHandle_t loopTaskHandle;

ArancinoMetadata amdata = {
  .fwname = "CS.2 - Smart_Light",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

char* keys[] = {"CS_2-Red", "CS_2-Green", "CS_2-Blue"};

void setup() {
  Arancino.begin(amdata);
  char* startingValues[] = {"255", "255", "255"}; //White
  Arancino.mset(keys, startingValues, 3);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop() {
  //empty
}

void loopTask(void *pvParameters){
  while (1){
    //Update RGB values
    char** result = Arancino.mget(keys,3);
    //Set newly fetched values
    setRGBValues(atoi(result[0]), atoi(result[1]), atoi(result[2]));
    Arancino.free(result);
    vTaskDelay(500);
  }
}

void setRGBValues(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
