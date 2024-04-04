#include "BG96.h"
#define SerialAT Serial1 //BG96 serial
#define SerialMon SerialUSB //DEBUG serial

#define DEBUG 1
bool BG96_powerReset()
{
  
  //pinMode(BG96_STA, INPUT);
  //pinMode(BG96_PWK, OUTPUT);
  digitalWrite(BG96_PWK, LOW);
  delay(1000);
  digitalWrite(BG96_PWK, HIGH);
  delay(1000);
  digitalWrite(BG96_PWK, LOW);
  while (digitalRead(BG96_STA))
  {
    SerialMon.println("wait off");
    delay(100);
  }
  SerialMon.println("BG96 powered OFF");
  digitalWrite(BG96_PWK, HIGH);
  delay(200);
  digitalWrite(BG96_PWK, LOW);
  delay(200);
  digitalWrite(BG96_PWK, HIGH);
  //delay(1000);
  while (!digitalRead(BG96_STA))
  {
    SerialMon.println("wait on");
    delay(100);
  }
  SerialMon.println("BG96 powered ON");
  return true;
}

bool BG96_powerOn()
{
  
  digitalWrite(BG96_PWK, HIGH);
  delay(1000);
  digitalWrite(BG96_PWK, LOW);
  delay(1000);
  digitalWrite(BG96_PWK, HIGH);
  //delay(10000);
  while (!digitalRead(BG96_STA))
  {
    SerialMon.println("wait on");
    delay(100);
  }
  //SerialMon.println("BG96 powered ON");
  return true;
}

/*bool BG96_powerOff(){
  digitalWrite(BG96_PWK, LOW);
  delay(1000);
  digitalWrite(BG96_PWK, HIGH);
  delay(1000);
  digitalWrite(BG96_PWK, LOW);
  while (digitalRead(BG96_STA))
  {
    SerialUSB.println("wait off");
    delay(100);
  }
  if(DEBUG)
    SerialMon.println("BG96 powered OFF");
  delay(5000);

  return true;
}*/

/*String acquireNMEA(String type)
{
  String response="";
  String cmd = QGPSGNMEA + String("\"");
  cmd += type;
  cmd += String("\"");
  cmd += (String)END_CMD;
  //SerialUSB.println(cmd);  
  SerialAT.println(cmd);
  response = SerialAT.readStringUntil('\n'); 
  SerialUSB.println("resp1 "+String(response));
  response = SerialAT.readStringUntil('\n');
  SerialUSB.println("resp2 "+String(response));
  String a=SerialAT.readStringUntil('\n');
  SerialUSB.println("uscita1"+a);
  String b=SerialAT.readStringUntil('\n');
  SerialUSB.println("uscita2"+b);
  response = response.substring(12);
  //SerialUSB.println("response: "+response);
  return response;
  
}*/

String acquireNMEA(String type)
{
  String cmd = QGPSGNMEA + String("\"");
  cmd += type;
  cmd += String("\"");
  cmd += (String)END_CMD;

  //String response = BG96_sendCommand(cmd, "$GP");
  String response = "";
  int startIndex;
  int endIndex;
  bool validResponse = false;
  unsigned long startMillis = millis();

  do {
    response = BG96_sendCommand(cmd, String(OK_RESPONSE), GNSS_TIMEOUT);
    startIndex = response.indexOf("$GP" + type);
    endIndex = response.indexOf(OK_RESPONSE);
    int errorIndex = response.indexOf("ERROR");

    if (startIndex > -1 && endIndex > -1 && response.indexOf("*") > -1) //check the presence of * for checksum calculation
    {
      validResponse = true;
      response = response.substring(startIndex, endIndex);
    }
    if(errorIndex!=-1){
      SerialMon.println("reset "+errorIndex);
      //NVIC_reset();

    }
  } while (!validResponse && (millis() - startMillis) < GNSS_TIMEOUT);

  return response;
}




String BG96_sendCommand(const String& cmd, String string_to_wait, unsigned long timeout)
{
  String response;
  if (cmd != "")
  {
    if(DEBUG)
      printDebug("(DEBUG) SND > : ", cmd, " (END SND)\r\n");
  }
  else
  {
    if(DEBUG)
      printDebug("(DEBUG) NO SND, WAITING RECV", "", "\r\n");
  }
  SerialAT.flush();
  //SerialAT.println(cmd);

  for (int i = 0; i < cmd.length(); i++)
  {
    SerialAT.write(cmd[i]);
  }

  unsigned long startMillis = millis();
  bool found = false;
  int receivedChars = 0;
  bool pdpDeact = false;

  do 
  {
    if (SerialAT.available())
    {
      receivedChars++;
      //      SerialAT.print("receivedChars = ");
      //      SerialAT.println(receivedChars);
      if (receivedChars > 1000)
      {
        SerialAT.println("FATAL ERROR");
        return "";
      }

      startMillis = millis(); //the timeout interval starts from the last char receiving
      response += (char)SerialAT.read();

      if (string_to_wait != "" && response.indexOf(string_to_wait) >= 0)
      {
        found = true;
        timeout = 0;
      }

      if (response.indexOf((String)AT_QIURC_RESPONSE + (String)"\"pdpdeact\"") >= 0)
      {
        pdpDeact = true;
        timeout = 0;
      }
    }
    else
    {
      //      SerialAT.print('.');
    }
  } while (/*!found &&*/ (millis() - startMillis) < timeout);

  if (response.substring(0, cmd.length()) == cmd)
  {
    //If the echo is present then remove it
    response = response.substring(cmd.length()); //delete the echoed command from the response string
  }
  if(DEBUG)
    printDebug("(DEBUG) RECV < : ", response, " (END RECV)\r\n");
  return response;
}

int enableNMEASentences(int en)
{
  int ret;
  String cmd = QGPSCFG + String(en);
  cmd += (String)END_CMD;

  String response = BG96_sendCommand(cmd, String(OK_RESPONSE));

  if (response == String(OK_RESPONSE))
    ret = 0;
  else
    ret = 1;

  return ret;

}

int TurnOnGNSS()
{
  SerialMon.println("(DEBUG) - Accensione GNSS");
  int ret;
  String cmd = QGPS + String("=1");
  cmd += (String)END_CMD;
  //String response = BG96_sendCommand(cmd, "\r\n");
  String response = BG96_sendCommand(cmd,String(OK_RESPONSE));
  if (response == String(OK_RESPONSE) || response == String(CME_ERROR_response) + String(504) + "\r\n") //504 = session is ongoing (already activated). to improve!
    ret = 0;
  else
    ret = 1;
  return ret;

}

int TurnOffGNSS()
{
  SerialMon.println("(DEBUG) - Accensione GNSS");
  int ret;
  String cmd = QGPSEND;
  cmd += (String)END_CMD;
  //String response = BG96_sendCommand(cmd, "\r\n");
  String response = BG96_sendCommand(cmd,String(OK_RESPONSE));
  if (response == String(OK_RESPONSE)) //504 = session is ongoing (already activated). to improve!
    ret = 0;
  else
    ret = 1;
  return ret;

}

String BG96_sendCommand(const String & cmd, String string_to_wait)
{
  return BG96_sendCommand(cmd, string_to_wait, 30000);
}

void BG96_setEcho(int en)
{
  int ret;
  String cmd = ATE + String(en) + (String)END_CMD;
  BG96_sendCommand(cmd, String(OK_RESPONSE));
}

void BG96_setTAFormat(int mode)
{
  int ret;
  String cmd = ATV + String(mode) + (String)END_CMD;
  BG96_sendCommand(cmd, String(OK_RESPONSE));
}


void printDebug(const String startStr, const String& str, const String endStr)
{
#if DEBUG == 1
  SerialMon.print(startStr);
  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] >= 16)
    {
      SerialMon.print(str[i]);
    }
    else
    {
      SerialMon.print("<0x");
      SerialMon.print(str[i], HEX);
      SerialMon.print(">");
    }
  }
  SerialMon.print(endStr);
#endif
}
