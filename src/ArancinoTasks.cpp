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

#if defined(USEFREERTOS)

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

  DynamicJsonDocument* ArancinoTasks::cmd_doc;
  DynamicJsonDocument* ArancinoTasks::rsp_doc;

  bool ArancinoTasks::_identificationFlag;
  bool ArancinoTasks::_interoceptionFlag;
  bool ArancinoTasks::_heartbeatFlag; 

  ArancinoTasks::ArancinoTasks(){
  cmd_doc = new DynamicJsonDocument(CMD_DOC_SIZE);
  rsp_doc = new DynamicJsonDocument(RSP_DOC_SIZE);

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

  void ArancinoTasks::heartbeatTask(){
    uint8_t idSize = strlen(Arancino.id);
    char topic[idSize + 5]; //ID_SIZE + '_HBx' + '\0'
    strcpy(topic, Arancino.id);
    strcat(topic, "_HB0");

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
    Arancino.free(rsp);
  }

  void ArancinoTasks::identificationTask(){
    JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
    (*cmd_doc)["cmd"] = GET_COMMAND;
    JsonArray cmd_items = cmd_args.createNestedArray("items");
    uint8_t idSize = strlen(Arancino.id);
    char blink_key[idSize + 1 + strlen(BLINK_ID_KEY)];
    strcpy(blink_key, Arancino.id);
    char separator = '_';
    strncat(blink_key, &separator, 1);
    strcat(blink_key, BLINK_ID_KEY);

    cmd_items.add(blink_key);
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

      cmd_args = (*cmd_doc).createNestedObject("args");
      (*cmd_doc)["cmd"] = SET_COMMAND;
      cmd_items = cmd_args.createNestedArray("items");
      JsonObject items_obj = cmd_items.createNestedObject();
      items_obj["key"] = blink_key;
      items_obj["value"] = 0;

      cmd_cfg = (*cmd_doc).createNestedObject("cfg");
      cmd_cfg["pers"] = 1;
      cmd_cfg["type"] = "rsvd";
      cmd_cfg["ack"] = 0;
      
      ArancinoPacket rsp = Arancino.executeCommand((*cmd_doc), false, VOID_RESPONSE);
      (*cmd_doc).clear();
    }
    Arancino.free(rsp);
  }

  void ArancinoTasks::interoceptionTask(){
    char* ts = Arancino.getTimestamp();
    JsonObject cmd_args = (*cmd_doc).createNestedObject("args");
    JsonArray cmd_items = cmd_args.createNestedArray("items");
    JsonObject temp_obj = cmd_items.createNestedObject();
    JsonObject cmd_cfg = (*cmd_doc).createNestedObject("cfg");

    (*cmd_doc)["cmd"] = STORE_COMMAND;
    temp_obj["key"] = "TEMP";
    temp_obj["value"] = mcuTemp();
    temp_obj["ts"] = ts;
    cmd_cfg["ack"] = 0;
    cmd_cfg["type"] = "tse";

    #if defined(__SAMD21G18A__)
      JsonObject memtot_obj = cmd_items.createNestedObject();
      JsonObject memused_obj = cmd_items.createNestedObject();
      memtot_obj["key"] = "MEM_TOT";
      memtot_obj["value"] = configTOTAL_HEAP_SIZE;
      memtot_obj["ts"] = ts;
      memused_obj["key"] = "MEM_USED";
      memused_obj["value"] = configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize();
      memused_obj["ts"] = ts;
    #elif defined(ARDUINO_ARCH_RP2040)
      JsonObject memtot_obj = cmd_items.createNestedObject();
      JsonObject memused_obj = cmd_items.createNestedObject();
      memtot_obj["key"] = "MEM_TOT";
      memtot_obj["value"] = rp2040.getTotalHeap();
      memtot_obj["ts"] = ts;
      memused_obj["key"] = "MEM_USED";
      memused_obj["value"] = rp2040.getUsedHeap();
      memused_obj["ts"] = ts;
    #endif
    
    Arancino.executeCommand((*cmd_doc), false, ITEMS_RESPONSE);
    (*cmd_doc).clear();
  }

  void ArancinoTasks::serviceTask(void* pvParameters){
    pinMode(LED_BUILTIN,OUTPUT);

    while(1){
      //Heartbeat
      if(_heartbeatFlag){
        heartbeatTask();
        _heartbeatFlag = false;
      }

      //Blink id
      if(_identificationFlag){
        identificationTask();
        _identificationFlag = false;
      }

      // Interoception
      if (_interoceptionFlag){
        interoceptionTask();
        _interoceptionFlag = false;
      }

      vTaskDelay(1000);
    }
  }


  #if defined(__SAMD21G18A__)
    float ArancinoTasks::mcuTemp(){
      TemperatureZero tempZero = TemperatureZero();
      tempZero.init();
      return tempZero.readInternalTemperature();
    }
  #elif defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2)
    float ArancinoTasks::mcuTemp(){
      #ifdef ATEMP
      int32_t VRef;
        #ifdef __LL_ADC_CALC_VREFANALOG_VOLTAGE
          VRef= (__LL_ADC_CALC_VREFANALOG_VOLTAGE(analogRead(AVREF), LL_ADC_RESOLUTION));
        #else
          VRef= (VREFINT * ADC_RANGE / analogRead(AVREF)); // ADC sample to mV
        #endif
        #ifdef __LL_ADC_CALC_TEMPERATURE
          return (__LL_ADC_CALC_TEMPERATURE(VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
        #elif defined(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS)
          return (__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(AVG_SLOPE, V25, CALX_TEMP, VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
        #else
          return 0;
        #endif
      #else
        return 0;
      #endif
  }

  #elif defined (ARDUINO_ARANCINO_VOLANTE)
    float ArancinoTasks::mcuTemp(){
      return readCPUTemperature();
    }
  #else
    float ArancinoTasks::mcuTemp(){
      return 0;
    }
  #endif
#endif