/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2023 SmartMe.IO

  Authors:  Marco Calapristi <marco.calapristi@smartme.io>
            Dario Gogliandolo <dario.gogliandolo@smartme.io>

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
  MQTTs example via LTE

  This sketch shows how to use Arancino Protocol via LTE implementing mTLS coomunication in MQTT protocol.
  In this example a BG96 modem was used with the TinyGSM library however, as shown in the MQTT example,
  each client interface should work as intended.

  In order to implement the mTLS the SSLClient library was used. All libraries are listed below and please
  refer to the "certificates.h" header for more info on how to create your own certificates file

  TinyGSM: https://github.com/vshymanskyy/TinyGSM
  SSLClient: https://github.com/OPEnSLab-OSU/SSLClient
*/


#define TINY_GSM_MODEM_BG96
#define SerialAT Serial1 //BG96 serial
#define SerialMon SerialUSB //DEBUG serial
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS true

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <Arancino.h>
#include "certificates.h" // This file must be regenerated

// Set this accordingly to your LTE provider
const char apn[]      = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char* broker = "";

// my_cert and my_key are included in the certificate header file
SSLClientParameters mTLS = SSLClientParameters::fromPEM(my_cert, sizeof my_cert, my_key, sizeof my_key);


ArancinoMetadata amdata = {
  .fwname = "Mqtt Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
SSLClient gsmClientSSL(client, TAs, (size_t)TAs_NUM, A5);
MqttIface iface;

void setup() {  
  SerialMon.begin(115200);
  delay(10);
  
  // Set your broker and Arancino daemon info
  iface.setDaemonID("87654321");
  iface.setBrokerAddress(broker);
  iface.setUsername("user");
  iface.setPassword("pass");
  iface.setPort(8883);
  iface.setNetworkClient(gsmClientSSL);
  
  Arancino.attachInterface(iface);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Wait...");
  SerialAT.begin(115200);

  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }
#endif


  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }
#endif

  Arancino.enableDebugMessages(SerialUSB);
  Arancino.begin(amdata);

}

void loop() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        SerialMon.println("GPRS reconnected");
      }
    }
  }

  // Send some data to Arancino
  Arancino.set("foo", "bar");
  
  char* msg = Arancino.get("foo");
  Arancino.println(msg);
  Arancino.free(msg);
  
  Arancino.del("foo");

  //Some delay to avoid sending too much messages if you have limited traffic on your sim
  delay(2000);
}