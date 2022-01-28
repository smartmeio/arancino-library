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

  In order to do that you should set up a MqttIface to attach to Arancino Library as shown below
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

void setup()
{
  Ethernet.init(9);   // 9 is CS pin for ethernet shield. Change this accordingly to your setup
  Ethernet.begin(mac, ip);
  
  // Allow the hardware to sort itself out
  delay(1500);

  //Set up the MQTT client
  MqttIface iface;
  iface.daemonID = "D43mon";
  iface.broker = "192.168.1.191"; //You can use domain names as well"
  //The folowing parameters are already set up with default values, but you can change them if your broker requires so
  iface.username = NULL;
  iface.password = NULL;
  iface.port = 1883;

  iface.setNetworkClient(&ethClient);
  Arancino.attachInterface(&iface);
  Arancino.begin(amdata);
}

void loop() {
  Arancino.set("Test_key", "my_value");
  delay(1000);
}