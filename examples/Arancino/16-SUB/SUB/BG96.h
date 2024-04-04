#ifndef BG96_H_
#define BG96_H_

#include <Arduino.h>

//#define SerialAT Serial
//#define BG96_STA  A0 
//#define BG96_PWK 12 
#define BG96_STA  A1 
#define BG96_PWK 38 

const char QGPS[]     =   "AT+QGPS";

const char QISTATE[]  =   "AT+QISTATE";
const char QGPSEND[]  =   "AT+QGPSEND";
const char QGPSLOC[]  =   "AT+QGPSLOC";
const char QGPSCFG[]  =   "AT+QGPSCFG=\"nmeasrc\",";
const char QGPSGNMEA[] =   "AT+QGPSGNMEA=";
const char CME_ERROR[] =   "+CME ERROR: ";
const char END_CMD[]  = "\r";
const int GNSS_TIMEOUT = 5000;
const int BG96_TIMEOUT = 1000;
const char OK_RESPONSE[]  =  "\r\nOK\r\n";
const char AT_QIURC_RESPONSE[] = "+QIURC: ";
const String CME_ERROR_response = "\r\n+CME ERROR: ";
const char AT[]     = "AT";
const char ATE[]     = "ATE"; //Set Command Echo Mode: ATE<value>
const char ATV[]     = "ATV"; //Set TA Response Format: ATV<value>
const int INIT_TIMEOUT = 30000;
String BG96_sendCommand(const String& cmd, String string_to_wait, unsigned long timeout);
String acquireNMEA(String type);
String BG96_sendCommand(const String& cmd, String string_to_wait);
bool BG96_powerReset();
bool BG96_powerOff();
bool BG96_powerOn();
int enableNMEASentences(int en);
int TurnOnGNSS();
int TurnOffGNSS();
void printDebug(const String startStr, const String& str, const String endStr);
void BG96_setEcho(int en);
void BG96_setTAFormat(int mode);

#endif
