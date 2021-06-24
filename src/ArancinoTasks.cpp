/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2018 SmartMe.IO

Authors:  Andrea Cannistrà  <andrea@smartme.io>

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

#include <ArancinoTasks.h>
#include <TemperatureZero.h>
#include <avr/dtostrf.h>

/******** TASK DEVICE IDENTIFICATION *********/

#if defined(USEFREERTOS)
void ArancinoTasks::deviceIdentification(void *pvPramaters){
  pinMode(LED_BUILTIN,OUTPUT);
  while (1)
  {
    char* value = Arancino.getBlinkId();
    int val = atoi(value);
    if(val){
        for(int i=0;i < 20; i++){
            digitalWrite(LED_BUILTIN,HIGH);
            vTaskDelay(100);
            digitalWrite(LED_BUILTIN,LOW);
            vTaskDelay(200);
        }
        Arancino.setBlinkId(0);
    }
    Arancino.free(value);
    vTaskDelay(10000); //wait 10 seconds (non-blocking delay)
  }
}

void ArancinoTasks::interoception(void *pvPramaters){
  TemperatureZero TempZero = TemperatureZero();
  TempZero.init();
  while (1)
  { 
    //free memory
    int memory_free = xPortGetFreeHeapSize();
    char mem_free[20];
    itoa(memory_free,mem_free,10);
    //mcu temperature
    float temperature = TempZero.readInternalTemperature();
    char temp[20];
    dtostrf(temperature,4,2,temp);
    //total memory
    char mem_tot[20];
    itoa(configTOTAL_HEAP_SIZE, mem_tot, 10);
    int memory_used=configTOTAL_HEAP_SIZE-memory_free;
    //used memory
    char mem_used[20];
    itoa(memory_used, mem_used, 10);
    char* keys[] = {"MEM_FREE","MEM_USED","MEM_TOT","TEMP"};
    char* values[] = {mem_free,mem_used,mem_tot,temp};
    Arancino.mstore(keys,values,4);
    vTaskDelay(60000); //wait 60 seconds (non-blocking delay)
  }

}
#endif