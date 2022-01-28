/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2022 SmartMe.IO

  Authors:  Marco Manfrè <marco@smartme.io>

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
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Arancino.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble

ArancinoMetadata amdata = {
  .fwname = "Bluetooth Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup()
{
  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  bledfu.begin();

  bledis.setManufacturer("SmartMe.IO");
  bledis.setModel("Arancino Volante");
  bledis.begin();
  
  bleuart.begin();

  startAdv();

  //Now that BLE Serial is activated we can connect it to the Arancino interface
  BluetoothIface iface;
  iface.setBLESerial(&bleuart);
  Arancino.attachInterface(&iface);

  Arancino.begin(amdata);  
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void startAdv(void)
{
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loopTask(void *pvParameters)
{
  while (1) {
    Arancino.set("Test_key", "my_value");
    vTaskDelay(1000); //wait 1 second
  }
}

void loop(){ 
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
}