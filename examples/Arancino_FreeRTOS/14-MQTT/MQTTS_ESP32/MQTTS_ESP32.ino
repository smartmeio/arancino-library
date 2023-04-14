/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2023 SmartMe.IO

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

#include <WiFi.h>
#include <Arancino.h>
#include <WiFiClientSecure.h>

// Change these values accordingly with your network.
const char* ssid = "ssid";
const char* password = "YourPassword!";

WiFiClientSecure arancinoClient;

MqttIface iface;

const char *test_root_ca =  \
    "-----BEGIN CERTIFICATE-----\n" \
    "-----END CERTIFICATE-----\n";

const char *test_client_key = \
    "-----BEGIN RSA PRIVATE KEY-----\n" \
    "-----END RSA PRIVATE KEY-----\n";

const char *test_client_cert = \
    "-----BEGIN CERTIFICATE-----\n" \
    "-----END CERTIFICATE-----\n";

ArancinoMetadata amdata = {
  .fwname = "Mqtt ESP32 w/ FreeRTOS Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

//FreeRtos
TaskHandle_t otherLoopHandle;
void otherLoop(void *pvParameters);

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting to " + (String)ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Load the certificates to network client
  arancinoClient.setCACert(test_root_ca);
  arancinoClient.setCertificate(test_client_cert);
  arancinoClient.setPrivateKey(test_client_key);

  Arancino.enableDebugMessages(Serial);
  Arancino.println("Started");

  //Set up the MQTT client
  iface.setDaemonID("87654321");
  iface.setBrokerAddress("192.168.211.41"); //You can use domain names as well
  iface.setUsername("arancino-daemon");
  iface.setPassword("12345678");
  iface.setPort(1883);
  iface.setNetworkClient(arancinoClient);
  Arancino.attachInterface(iface);

  Arancino.begin(amdata);
  xTaskCreate(otherLoop, "otherLoop", configMINIMAL_STACK_SIZE * 2, NULL, 1, &otherLoopHandle);
  Arancino.startScheduler();
}

void loop() {
  //Check if WiFi is still connected
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
  }

  Arancino.set("foo", "bar");
  Arancino.println("SET: foo -> bar");
  delay(1000);
}

void otherLoop(void *pvParameters) {
  while (1) {
    char* value = Arancino.get("foo");
    Arancino.print("GET: foo -> ");
    Arancino.println(value);
    Arancino.free(value); //frees memory

    delay(1000);
  }
}