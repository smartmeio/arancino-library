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
 * CS_1 Sample Sensor
 * This case scenario shows how to sample data from a temperature sensor (eg. TMP36)
 * and how to take full advantage of Arancino Library for storing and manipulating data
 */

 #include <Arancino.h>

//Arancino interface
SerialIface iface;

 #define sensePin A0
 #define SAMPLES 10

ArancinoMetadata amdata = {
  .fwname = "CS.1 - Sample_Sensor",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup() {
  iface.setSerialPort();
  Arancino.attachInterface(iface);
  Arancino.begin(amdata);

  xTaskCreate(loopTask, "loopTask", 512, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop() {
  //empty
}

void loopTask(void *pvParameters){
  while(1){
    sampleData();
    generateResults();
    delay(10000);
  }
}

void sampleData(){
  int data[SAMPLES];

  //Register a sample every 10 milliseconds
  for (int i=0; i<SAMPLES; i++){
    data[i] = analogRead(sensePin);
    delay(10);
  }

  //Store samples inside an hash table
  for (int i=0; i<SAMPLES; i++){
    //Field must be char* so it must be converted first
    char buff[5];
    itoa(i, buff, 10);

    Arancino.hset("samples", buff, data[i]);
  }
}

void generateResults(){
  float tempValues[SAMPLES];
  float mean = 0, sqDevSum = 0;
  char* value;;

  //Convert stored values (analog read) to temperature degrees
  for (int i=0; i<SAMPLES; i++){
    char buff[5];
    itoa(i, buff, 10);

    value = Arancino.hget("samples", buff);

    tempValues[i] = atof(value)/1024;  // percentage of input reading
    tempValues[i] = tempValues[i] * 5;  // get voltage
    tempValues[i] = tempValues[i] - 0.5;  // subtract the offset
    tempValues[i] = tempValues[i]  * 100;  // convert to degrees
  }

  // mean calculation
  for (int i=0; i<SAMPLES; i++){
    mean+=tempValues[i];
  }
  mean /= SAMPLES;

  // standard deviation calculation
  for (int i=0; i<SAMPLES; i++){
    sqDevSum += pow((mean - tempValues[i]), 2);
  }
  float stdDev = sqrt(sqDevSum/SAMPLES);

  // Save generated results alongside their timestamp
  Arancino.free(value);
  Arancino.store("Means", mean);
  Arancino.store("StdDevs", stdDev);
}
