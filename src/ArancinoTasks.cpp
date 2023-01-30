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
#if defined(__SAMD21G18A__)
#include <TemperatureZero.h>
#endif
#if defined(ARDUINO_ARCH_ESP32)
#include <stdlib.h>
#else
#include <avr/dtostrf.h>
#endif
#if defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2)
#include "stm32yyxx_ll_adc.h"
#define CALX_TEMP 25
#define LL_ADC_RESOLUTION LL_ADC_RESOLUTION_12B
#define ADC_RANGE 4096
#endif

/******** TASK DEVICE IDENTIFICATION *********/

#if defined(USEFREERTOS)

FreeRTOSDynamicJsonDocument* ArancinoTasks::cmd_doc;
FreeRTOSDynamicJsonDocument* ArancinoTasks::rsp_doc;
// SemaphoreHandle_t ArancinoTasks::jsonMutex;
bool ArancinoTasks::_identificationFlag;
bool ArancinoTasks::_interoceptionFlag;
bool ArancinoTasks::_heartbeatFlag; 

ArancinoTasks::ArancinoTasks(){
  cmd_doc = new FreeRTOSDynamicJsonDocument(CMD_DOC_SIZE);
  rsp_doc = new FreeRTOSDynamicJsonDocument(RSP_DOC_SIZE);
  //jsonMutex = xSemaphoreCreateMutex();

  _identificationFlag = false;
  _interoceptionFlag = false;
  _heartbeatFlag = false; 
}

void ArancinoTasks::identificationCallback (TimerHandle_t xTimer){
  ArancinoTasks::_identificationFlag = true;
}

void ArancinoTasks::interoceptionCallback (TimerHandle_t xTimer){
  ArancinoTasks::_interoceptionFlag = true;
}

void ArancinoTasks::heartbeatCallback (TimerHandle_t xTimer){
  _heartbeatFlag = true;
}


void ArancinoTasks::serviceTask(void* pvParameters){
  //Heartbeat region
  uint8_t idSize = strlen(Arancino.id);
  char topic[idSize + 5]; //ID_SIZE + '_HBx' + '\0'
  strcpy(topic, Arancino.id);
  strcat(topic, "_HB0");

  //Identification region
  pinMode(LED_BUILTIN,OUTPUT);

  // digitalWrite(LED_BUILTIN, HIGH);
  // Arancino.delay(200);
  // digitalWrite(LED_BUILTIN, LOW);
  // Arancino.delay(200);
  

  while(1){

    if(_heartbeatFlag){
      topic[idSize+3] = '0'; //If the topic structure isn't changed i know exactly the byte I'm supposed to change

      JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
      (*cmd_doc)["cmd"] = PUBLISH_COMMAND;
      JsonArray cmd_items = cmd_args.createNestedArray("items");
      JsonObject items_obj = cmd_items.createNestedObject();
      items_obj["channel"] = topic;
      items_obj["message"] = Arancino.getTimestamp();

      JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
      cmd_cfg["ack"] = 0;
      cmd_cfg["pers"] = 0;

      ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), (*rsp_doc), false, CLIENTS_RESPONSE);
      
      topic[idSize+3] = '1'; //If the topic structure isn't changed i know exactly the byte I'm supposed to change

      items_obj["channel"] = topic;
      items_obj["message"] = Arancino.getTimestamp();

      rsp = Arancino.executeCommand((*cmd_doc), (*rsp_doc), false, CLIENTS_RESPONSE);
      (*cmd_doc).clear();
      _heartbeatFlag = false;
    }

/*


    if(_identificationFlag){
      JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
      (*cmd_doc)["cmd"] = GET_COMMAND;
      JsonArray cmd_items = cmd_args.createNestedArray("items");
      cmd_items.add(BLINK_ID_KEY);

      JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
      cmd_cfg["pers"] = 1;
      cmd_cfg["type"] = "rsvd";
      
      ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), true, KEY_VALUE_RESPONSE);
      (*cmd_doc).clear();

      if(!strcmp(rsp.response.string, "1")){
        for(int i=0;i < 20; i++){
            #if defined (ARDUINO_ARANCINO_VOLANTE)
            digitalWrite(LED_BUILTIN,LOW);
            #else
            digitalWrite(LED_BUILTIN,HIGH);
            #endif
            vTaskDelay(100);
            #if defined (ARDUINO_ARANCINO_VOLANTE)
            digitalWrite(LED_BUILTIN,HIGH);
            #else
            digitalWrite(LED_BUILTIN,LOW);
            #endif
            vTaskDelay(200);
        }

        JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
        (*cmd_doc)["cmd"] = SET_COMMAND;
        JsonArray cmd_items = cmd_args.createNestedArray("items");
        JsonObject items_obj = cmd_items.createNestedObject();
        items_obj["key"] = BLINK_ID_KEY;
        items_obj["value"] = 0;

        JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
        cmd_cfg["pers"] = 1;
        cmd_cfg["type"] = "rsvd";
        cmd_cfg["ack"] = 0;
        
        ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, VOID_RESPONSE);
        (*cmd_doc).clear();
        _identificationFlag = false;
      }

      if (_interoceptionFlag){
        #if !defined(ARDUINO_ARANCINO_VOLANTE) && !defined(ARDUINO_ARCH_RP2040)
        int memory_free = xPortGetFreeHeapSize();
        int memory_used=configTOTAL_HEAP_SIZE-memory_free;
        #endif

        #if defined(ARDUINO_ARANCINO_VOLANTE) || defined(ARDUINO_ARCH_RP2040)
        char* ts = Arancino.getTimestamp();
        JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
        (*cmd_doc)["cmd"] = STORE_COMMAND;
        JsonArray cmd_items = cmd_args.createNestedArray("items");
        JsonObject memfree_obj = cmd_items.createNestedObject();
        JsonObject temp_obj = cmd_items.createNestedObject();
        
        memfree_obj["key"] = "MEM_TOT";
        memfree_obj["value"] = configTOTAL_HEAP_SIZE;
        memfree_obj["ts"] = ts;
        temp_obj["key"] = "TEMP";
        temp_obj["value"] = mcuTemp();
        temp_obj["ts"] = ts;
        JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
        cmd_cfg["ack"] = 0;
        cmd_cfg["type"] = "tse";
        
        ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, ITEMS_RESPONSE);
        (*cmd_doc).clear();
        #else
        char* ts = Arancino.getTimestamp();
        JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
        (*cmd_doc)["cmd"] = STORE_COMMAND;
        JsonArray cmd_items = cmd_args.createNestedArray("items");
        JsonObject memfree_obj = cmd_items.createNestedObject();
        JsonObject temp_obj = cmd_items.createNestedObject();
        JsonObject memtot_obj = cmd_items.createNestedObject();
        JsonObject memused_obj = cmd_items.createNestedObject();
        
        memtot_obj["key"] = "MEM_TOT";
        memtot_obj["value"] = configTOTAL_HEAP_SIZE;
        memtot_obj["ts"] = ts;
        temp_obj["key"] = "TEMP";
        temp_obj["value"] = 0;
        temp_obj["ts"] = ts;
        memfree_obj["key"] = "MEM_FREE";
        memfree_obj["value"] = xPortGetFreeHeapSize();
        memfree_obj["ts"] = ts;
        memused_obj["key"] = "MEM_USED";
        memused_obj["value"] = configTOTAL_HEAP_SIZE-memory_free;
        memused_obj["ts"] = ts;
        JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
        cmd_cfg["ack"] = 0;
        cmd_cfg["type"] = "tse";
        
        ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, ITEMS_RESPONSE);
        (*cmd_doc).clear();
      }
    }

*/
    //METTERE LO YIELD
    vTaskDelay(1000);
  }
}


// void ArancinoTasks::deviceIdentification(void *pvPramaters){
//   pinMode(LED_BUILTIN,OUTPUT);
//   while (1)
//   {
//     xSemaphoreTake(jsonMutex, (TickType_t)portMAX_DELAY);
//     JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
//     (*cmd_doc)["cmd"] = GET_COMMAND;
//     JsonArray cmd_items = cmd_args.createNestedArray("items");
//     cmd_items.add(BLINK_ID_KEY);

//     JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
//     cmd_cfg["pers"] = 1;
//     cmd_cfg["type"] = "rsvd";
    
//     ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), true, KEY_VALUE_RESPONSE);
//     xSemaphoreGive(jsonMutex);


//     if(!strcmp(rsp.response.string, "1")){
//         for(int i=0;i < 20; i++){
//             #if defined (ARDUINO_ARANCINO_VOLANTE)
//             digitalWrite(LED_BUILTIN,LOW);
//             #else
//             digitalWrite(LED_BUILTIN,HIGH);
//             #endif
//             vTaskDelay(100);
//             #if defined (ARDUINO_ARANCINO_VOLANTE)
//             digitalWrite(LED_BUILTIN,HIGH);
//             #else
//             digitalWrite(LED_BUILTIN,LOW);
//             #endif
//             vTaskDelay(200);
//         }

//         xSemaphoreTake(jsonMutex, (TickType_t)portMAX_DELAY);
//         JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
//         (*cmd_doc)["cmd"] = SET_COMMAND;
//         JsonArray cmd_items = cmd_args.createNestedArray("items");
//         JsonObject items_obj = cmd_items.createNestedObject();
//         items_obj["key"] = BLINK_ID_KEY;
//         items_obj["value"] = 0;
//         //cmd_items.add(BLINK_ID_KEY);

//         JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
//         cmd_cfg["pers"] = 1;
//         cmd_cfg["type"] = "rsvd";
//         cmd_cfg["ack"] = 0;
        
//         ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, VOID_RESPONSE);
//         (*cmd_doc).clear();
//         xSemaphoreGive(jsonMutex);

//     }
//     Arancino.free(rsp);
//     vTaskDelay(10000); //wait 10 seconds (non-blocking delay)
//   }
// }

// void ArancinoTasks::interoception(void *pvPramaters){
//   while (1)
//   {
//     #if !defined(ARDUINO_ARANCINO_VOLANTE) && !defined(ARDUINO_ARCH_RP2040)
//     int memory_free = xPortGetFreeHeapSize();
//     int memory_used=configTOTAL_HEAP_SIZE-memory_free;
//     #endif

//     #if defined(ARDUINO_ARANCINO_VOLANTE) || defined(ARDUINO_ARCH_RP2040)
//     xSemaphoreTake(jsonMutex, (TickType_t)portMAX_DELAY);
//     char* ts = Arancino.getTimestamp();
//     JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
//     (*cmd_doc)["cmd"] = STORE_COMMAND;
//     JsonArray cmd_items = cmd_args.createNestedArray("items");
//     JsonObject memfree_obj = cmd_items.createNestedObject();
//     JsonObject temp_obj = cmd_items.createNestedObject();
    
//     memfree_obj["key"] = "MEM_TOT";
//     memfree_obj["value"] = configTOTAL_HEAP_SIZE;
//     memfree_obj["ts"] = ts;
//     temp_obj["key"] = "TEMP";
//     temp_obj["value"] = mcuTemp();
//     temp_obj["ts"] = ts;
//     JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
//     cmd_cfg["ack"] = 0;
//     cmd_cfg["type"] = "tse";
    
//     ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, ITEMS_RESPONSE);
//     xSemaphoreGive(jsonMutex);

//     #else
//     xSemaphoreTake(jsonMutex, (TickType_t)portMAX_DELAY);
//     char* ts = Arancino.getTimestamp();
//     JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
//     (*cmd_doc)["cmd"] = STORE_COMMAND;
//     JsonArray cmd_items = cmd_args.createNestedArray("items");
//     JsonObject memfree_obj = cmd_items.createNestedObject();
//     JsonObject temp_obj = cmd_items.createNestedObject();
//     JsonObject memtot_obj = cmd_items.createNestedObject();
//     JsonObject memused_obj = cmd_items.createNestedObject();
    
//     memtot_obj["key"] = "MEM_TOT";
//     memtot_obj["value"] = configTOTAL_HEAP_SIZE;
//     memtot_obj["ts"] = ts;
//     temp_obj["key"] = "TEMP";
//     temp_obj["value"] = 0;
//     temp_obj["ts"] = ts;
//     memfree_obj["key"] = "MEM_FREE";
//     memfree_obj["value"] = xPortGetFreeHeapSize();
//     memfree_obj["ts"] = ts;
//     memused_obj["key"] = "MEM_USED";
//     memused_obj["value"] = configTOTAL_HEAP_SIZE-memory_free;
//     memused_obj["ts"] = ts;
//     JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
//     cmd_cfg["ack"] = 0;
//     cmd_cfg["type"] = "tse";
    
//     ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, ITEMS_RESPONSE);
//     (*cmd_doc).clear();
//     xSemaphoreGive(jsonMutex);
//     #endif
//     Arancino.free(rsp);
//     vTaskDelay(60000); //wait 60 seconds (non-blocking delay)
//   }

// }

// void ArancinoTasks::sendHeartbeat(void *pvPramaters){
//   uint8_t idSize = strlen(Arancino.id);
//   char topic[idSize + 5]; //ID_SIZE + '_HBx' + '\0'
//   strcpy(topic, Arancino.id);
//   strcat(topic, "_HB0");

//   while (1) {
//     xSemaphoreTake(jsonMutex, (TickType_t)portMAX_DELAY);
//     topic[idSize+3] = '0'; //If the topic structure isn't changed i know exactly the byte I'm supposed to change
    
//     JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
//     (*cmd_doc)["cmd"] = PUBLISH_COMMAND;
//     JsonArray cmd_items = cmd_args.createNestedArray("items");
//     JsonObject items_obj = cmd_items.createNestedObject();
//     items_obj["channel"] = topic;
//     items_obj["message"] = Arancino.getTimestamp();

//     JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");
//     cmd_cfg["ack"] = 0;
//     cmd_cfg["pers"] = 0;

//     ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), (*rsp_doc), false, CLIENTS_RESPONSE);
    
//     topic[idSize+3] = '1'; //If the topic structure isn't changed i know exactly the byte I'm supposed to change

//     items_obj["channel"] = topic;
//     items_obj["message"] = Arancino.getTimestamp();

//     rsp = Arancino.executeCommand((*cmd_doc), (*rsp_doc), false, CLIENTS_RESPONSE);
//     (*cmd_doc).clear();
//     xSemaphoreGive(jsonMutex);

//     vTaskDelay(10000);
//   }
// }

// #if defined(__SAMD21G18A__)
// float ArancinoTasks::mcuTemp(){
//   TemperatureZero tempZero = TemperatureZero();
//   tempZero.init();
//   return tempZero.readInternalTemperature();
// }

// #elif defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2)


// float ArancinoTasks::mcuTemp(){
//   #ifdef ATEMP
//   int32_t VRef;
//     #ifdef __LL_ADC_CALC_VREFANALOG_VOLTAGE
//        VRef= (__LL_ADC_CALC_VREFANALOG_VOLTAGE(analogRead(AVREF), LL_ADC_RESOLUTION));
//     #else
//        VRef= (VREFINT * ADC_RANGE / analogRead(AVREF)); // ADC sample to mV
//     #endif
//     #ifdef __LL_ADC_CALC_TEMPERATURE
//       return (__LL_ADC_CALC_TEMPERATURE(VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
//     #elif defined(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS)
//       return (__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(AVG_SLOPE, V25, CALX_TEMP, VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
//     #else
//       return 0;
//     #endif
//   #else
//     return 0;
//   #endif

// }

// #elif defined (ARDUINO_ARANCINO_VOLANTE)

// float ArancinoTasks::mcuTemp(){
//   return readCPUTemperature();
// }

// #else

// float ArancinoTasks::mcuTemp(){
//   return 0;
// }
// #endif
#endif
