/*
  SPDX-license-identifier: Apache-2.0

  Copyright (C) 2022 SmartMe.IO

  Authors:  Andrea Cannistra <andrea.cannistra@smartme.io>

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

Returns events containing messages related to the subscribed channels.

- ArancinoPacket subscribe(char** channel, void (*callbacks)(ArancinoPacket), int len)

Parameters:
* `channel`: array that contains the subcribed redis channel;
* `callback`: callback function that receives messages from the subscribed channel;
* `len`: number of channel subscribed.

To activate the callback, you need to implement the `Arancino.loop` function, 
which iterates for new events on the subscribed channel.

This examples is compatible with BG96 modem and samd21 core 

*/

#define TINY_GSM_MODEM_BG96

#include <Arancino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#define SerialMon SerialUSB 
#define SerialAT Serial

#include "BG96.h"
//enable debug messages
#define DEBUG 1

MqttIface iface;
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

//setup your apn
const char apn[] = "ibox.tim.it";
const char gprsUser[] = "";
const char gprsPass[] = "";

ArancinoMetadata amdata = {
  .fwname = "Subscribe Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};


void callbackTopic(ArancinoPacket pckt) {

        SerialMon.print("Topic: ");
        SerialMon.print(pckt.response.stringArray[0]);
        SerialMon.print(" Message: ");
        SerialMon.println(pckt.response.stringArray[1]);
        Arancino.free(pckt);
}


void setup()
{
  SerialMon.begin(115200);
  //setup BG96 modem
  pinMode(BG96_STA, INPUT_PULLDOWN);
  pinMode(BG96_PWK, OUTPUT);
  modem_setup();
  //Set up the MQTT client
  iface.setDaemonID("00000000e52ac8ed");
  iface.setBrokerAddress("vulcano.mqtt.smartme.io"); //You can use the IP address
  iface.setUsername("arancini");
  iface.setPassword("smartme");
  iface.setPort(1883);
  iface.setNetworkClient(client);
  Arancino.attachInterface(iface);

  Arancino.begin(amdata);

  SerialMon.begin(115200);
  //redis channel subscribed 
  char* channels[] = {"channel1","channel2"};
  Arancino.subscribe(channels,callbackTopic,2);
}

void loop() 
{
  //iterates for new events on the subscribed channel
  Arancino.loop();
}

void modem_setup()
{
    bool BG96_power = BG96_powerOn();
    
    if(BG96_power)
      if(DEBUG)
        SerialMon.println("BG96 Power: ON");

    if(DEBUG)
      SerialMon.println("Wait...");
    SerialAT.begin(115200);

    if(DEBUG)
      SerialMon.println("Initializing modem...");

    modem.restart();
    String modemInfo = modem.getModemInfo();
    if(DEBUG)
    {
      SerialMon.print("Modem Info: ");
      SerialMon.println(modemInfo);
    }
    
    #if TINY_GSM_USE_GPRS
      // Unlock your SIM card with a PIN if needed
      if (GSM_PIN && modem.getSimStatus() != 3) 
      {
        modem.simUnlock(GSM_PIN);
      }
    #endif

    modem.gprsConnect(apn, gprsUser, gprsPass);

    if(DEBUG)
      SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) 
    {
      if(DEBUG)
        SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if(DEBUG)
      SerialMon.println(" success");

    if (modem.isNetworkConnected()) 
    {
      if(DEBUG)
        SerialMon.println("Network connected");
    }

 
    // GPRS connection parameters are usually set after network registration
    if(DEBUG)
    {
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
    }
    if (!modem.gprsConnect(apn, gprsUser,gprsPass)) 
    {
      if(DEBUG)
        SerialMon.println(" fail");
      delay(10000);
      return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) 
    {
      if(DEBUG)
        SerialMon.println("GPRS connected");
    }
}
