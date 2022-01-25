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
  Arancino MQTT example

  This sketch shows how to use Arancino Protocol via MQTT.
  In this example an Ethernet shield was used, but any network client should work just fine.

  Inside the sketch the ArancinoConfig should be set as shown below. Every other operation can be performed as usual
  since communication is handled with an Hardware Abstaction Layer (which can be selected from the Interface menu)
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Arancino.h>

// Update these with values suitable for your network.
byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };    //MAC address of ethernet shield
IPAddress ip(192, 168, 1, 100);   // Static IP Address. You can also use your DHCP server to automatically assign one

EthernetClient ethClient;

ArancinoMetadata amdata = {
  .fwname = "Mqtt Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);

void setup()
{
  Ethernet.init(9);   // 9 is CS pin for ethernet shield. Change this accordingly to your setup
  Ethernet.begin(mac, ip);
  
  // Allow the hardware to sort itself out
<<<<<<< HEAD
  vTaskDelay(1500);
=======
  delay(1500);
>>>>>>> ea60fa9236c9108b48fba5dde67486693a789ec0

  //When setting up the configurator, please make sure that iface parameters correctly match the selected interface
  //Please check out the documentation for further details
  ArancinoConfig aconfig;
<<<<<<< HEAD
  aconfig.iface.setNetworkClient(&ethClient);
=======
  aconfig.iface.client = &ethClient;
>>>>>>> ea60fa9236c9108b48fba5dde67486693a789ec0
  aconfig.iface.daemonID = "D43mon";
  aconfig.iface.broker = "192.168.1.191";
  //The folowing parameters are already set up with default values, but you can change them if your broker requires so
  aconfig.iface.username = NULL;
  aconfig.iface.password = NULL;
  aconfig.iface.port = 1883;

  Arancino.begin(amdata, aconfig);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop()
{
}

void loopTask(void *pvParameters) {
  while (1) {
    Arancino.set("Test_key", "my_value");
    vTaskDelay(1000); //wait 1 second
  }
}