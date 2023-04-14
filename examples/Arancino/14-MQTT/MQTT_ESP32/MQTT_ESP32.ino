/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2023 SmartMe.IO

  Authors:  Dario Gogliandolo <dario.gogliandolo@smartme.io>

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

#include <WiFi.h>
#include <Arancino.h>

// Change these values accordingly with your network.
const char* ssid = "ssid";
const char* password = "YourPassword!";

WiFiClient arancinoClient;

MqttIface iface;

ArancinoMetadata amdata = {
  .fwname = "Mqtt ESP32 Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting to " + (String)ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Arancino.enableDebugMessages(Serial);
  Arancino.println("Started");

  //Set up the MQTT client
  iface.setDaemonID("87654321");
  iface.setBrokerAddress("192.168.137.249"); //You can use domain names as well
  iface.setUsername("arancino-daemon");
  iface.setPassword("12345678");
  iface.setPort(1883);
  iface.setNetworkClient(arancinoClient);
  Arancino.attachInterface(iface);

  Arancino.begin(amdata);
}

void loop() {
  //Check if WiFi is still connected
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
  }
  
  Arancino.set("foo", "bar");
  Arancino.println("SET: foo -> bar");
  delay(1000);
  
  char* value = Arancino.get("foo");
  Arancino.print("GET: foo -> ");
  Arancino.println(value);
  Arancino.free(value); //frees memory
  
  delay(1000);
}
