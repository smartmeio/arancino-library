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

  In order to do that you should first set up a MqttIface and then attach it to Arancino Library as shown below
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Arancino.h>

// Change these values accordingly with your network.
byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };    //MAC address of ethernet shield
IPAddress ip(192, 168, 1, 100);   // Static IP Address. You can also use your DHCP server to automatically assign one

EthernetClient ethClient;
MqttIface iface;

ArancinoMetadata amdata = {
  .fwname = "Mqtt Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t customLoopTaskHandle;
void customLoopTask(void *pvParameters);

void setup()
{
  SerialUSB.begin(115200);
  Ethernet.init(9);   //CS pin
  Ethernet.begin(mac, ip);
  
  // Allow the hardware to sort itself out
  delay(1500);

  Arancino.enableDebugMessages(SerialUSB);
  Arancino.println("Started");

  //Set up the MQTT client
  iface.setDaemonID("12345");
  iface.setBrokerAddress("192.168.1.10"); //You can use domain names as well
  iface.setUsername("Arancino");
  iface.setPassword("pwd");
  iface.setPort(1883);
  iface.setNetworkClient(ethClient);
  Arancino.attachInterface(iface);
  
  Arancino.begin(amdata);
  xTaskCreate(customLoopTask, "customLoopTask", 512, NULL, 1, &customLoopTaskHandle);
  Arancino.startScheduler();

}

void loop() {
  //empty
}

void customLoopTask(void *pvParameters){
  while(1){
    Arancino.set("foo", "bar");
    Arancino.println("foo -> bar");
    delay(2000);
  }
}