/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2022 SmartMe.IO

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
  Arancino Bluetooth example

  This sketch shows how to use Arancino Protocol via Bluetooth.
  This example is intended to work with Arancino Volante boards (nRF52), but you can provide any BLESerial object
  depending on your hardware to Arancino Protocol and it should work just fine.

  In order to establish a Bluetooth connection you should first configure your BLE Serial and only then attach it to
  the Arancino Interface as shown below.
*/

#include <Adafruit_TinyUSB.h>
#include <bluefruit.h>
#include <ArancinoBleSetup.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

#include <Arancino.h>

BluetoothIface iface;

ArancinoMetadata amdata = {
  .fwname = "Bluetooth Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup()
{
  //Now that BLE Serial is activated we can connect it to the Arancino interface
  ArancinoBleSetup(bleuart, bledis, bledfu);
  iface.setBLESerial(bleuart);
  Arancino.attachInterface(iface);

  Arancino.begin(amdata);
  xTaskCreate(loopTask, "loopTask", 512, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop(){
  //empty
}

void loopTask(void *pvParameters){
  while(1){
    Arancino.set("foo", "bar");
    vTaskDelay(2000);
  }
}