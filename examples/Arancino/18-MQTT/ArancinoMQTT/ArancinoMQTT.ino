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
 In this example an Ethernet shield was used but any network client should work just fine.

 Apart from declaring the configurator, Arancino protocol is going to work as intended since comunication is handled with an HAL
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Arancino.h>

// Update these with values suitable for your network.
byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };    //MAC address of ethernet shield
IPAddress ip(192, 168, 158, 187);   // Static IP Address. You can also use your DHCP server to automatically assign one
char server[] = "server.smartme.io";   // Broker IP Address

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
  Ethernet.init(9);   // 9 is CS pin for ethernet shield. Change this based on your setup
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

  // Declare configurator
  MqttConfig mqtt(ethClient, server, "port_id_1", "d43mon");
  ArancinoConfig aconfig;
  aconfig.mqttConfig = &mqtt;
  Arancino.begin(amdata, aconfig);
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();
}

void loop()
{
}

void loopTask(void *pvParameters){
  while(1){
    Arancino.set("Test_key", "my_value");
    vTaskDelay(500); //wait 0.5 seconds
  }
}