/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2018 SmartMe.IO

Authors:  Andrea Cannistrà <andrea@smartme.io>

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

#ifndef ARANCINO_H_
#define ARANCINO_H_

#include <Arduino.h>
#include <ArancinoDefinitions.h>
#include <ArancinoConfig.h>
#include <ArancinoInterface.h>
#include <Stream.h>
#include <stdlib.h>
#include <cstdlib>
#include <type_traits>
#include <MicrocontrollerID.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <stdlib.h>
#else
#include <avr/dtostrf.h>
#endif

#include <ArduinoJson.h>

//#define USEFREERTOS
#if defined(USEFREERTOS)
#if defined(__SAMD21G18A__)

	// #if !defined(__MEM_WRAP__)
	// 	#error You are using Arancino Library, please select Menu -> Tools -> Using Arancino Library?: Yes
	// #endif

extern "C" {
#include <FreeRTOS_SAMD21.h>
}

#elif defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2)

extern "C" {
#include <STM32FreeRTOS.h>
}
#elif defined (ARDUINO_ARANCINO_VOLANTE)
//need to serial
#include <Adafruit_TinyUSB.h>
#elif defined (ARDUINO_ARCH_RP2040)
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <semphr.h>
#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h>
#endif
#endif


#endif//end USEFREERTOS

//RP2040
#if defined(ARDUINO_ARCH_RP2040)
#include <hardware/adc.h>
#include <hardware/watchdog.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
#include <esp_task_wdt.h>
#define pvPortMalloc std::malloc
#define vPortFree std::free
#endif

#include "ArancinoTasks.h"

//Power Mode
enum POWER_MODE {
	BATTERY = 0,
	POWERSUPPLY = 1
};

typedef union {
  int integer;
  char* string;
  char** stringArray;
} ArancinoResponse;

typedef struct {
  bool isError;
  int responseCode;
  int responseType;
  ArancinoResponse response;
} ArancinoPacket;

typedef struct {
	const char* fwname;
	const char* fwversion;
	const char* tzoffset;
} ArancinoMetadata;

typedef struct {
	uint8_t pers = CFG_UNSET;
	uint8_t ack = CFG_UNSET;
	const char* type = CFG_UNSET;
	//const char* signature
} ArancinoCFG;





class ArancinoClass {
	public:
		/***** API ADVANCED *****/

		//Serial port id
		char id[ID_SIZE+1];

		//START SCHEDULER
		void startScheduler();

		//SLEEP
		void delay(unsigned long milli);

		/***** API BASIC *****/

		//ATTACH_INTERFACE
		void attachInterface(ArancinoIface& iface);

		//BEGIN
		void begin(ArancinoMetadata _amdata, ArancinoConfig _acfg, const char* custom_v1 = NULL, const char* custom_v2 = NULL);
		void begin(ArancinoMetadata _amdata, const char* custom_v1 = NULL, const char* custom_v2 = NULL);

		ArancinoPacket set(const char* key, int value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(const char* key, double value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(const char* key, float value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(const char* key, long value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(const char* key, uint32_t value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(const char* key, const char* value, bool isAck = true, bool isPersistent = false, const char* type = "appl");

		//MSET
		ArancinoPacket mset(char** keys, char** values, int len, bool isAck=true, bool isPersistent = false, const char* type = "appl");

		//GET
		template<class T = char*> T get(const char* key, bool isPersistent = false, const char* type = "appl");

		//MGET
		template<class T = char**> T mget(char** keys, int len, bool isPersistent = false, const char* type = "appl");

		//DEL
		template<class T = int> T del(const char* key, bool isAck = true, bool isPersistent = false, const char* type = "appl");

		//HSET
		ArancinoPacket hset(const char* key, const char* field, const char* value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(const char* key, const char* field, int value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(const char* key, const char* field, float value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(const char* key, const char* field, double value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(const char* key, const char* field, uint32_t value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(const char* key, const char* field, long value, bool isAck = true, bool isPersistent = false, const char* type = "appl");

		//HGET
		template<class T = char*> T hget(const char* key, const char* field, bool isPersistent = false, const char* type = "appl");

		//HDEL
		template<class T = int> T hdel(const char* key, const char* field, bool isAck = true, bool isPersistent = false);

		//PUBLISH
		template<class T = int> T publish(const char* channel, const char* msg, bool isAck = true);
		template<class T = int> T publish(const char* channel, double msg, bool isAck = true);
		template<class T = int> T publish(const char* channel, int msg, bool isAck = true);
		template<class T = int> T publish(const char* channel, uint32_t msg, bool isAck = true);
		template<class T = int> T publish(const char* channel, long msg, bool isAck = true);

		//FLUSH
		ArancinoPacket flush(bool isAck = true, bool isPersistent = false);

		//STORE
		template<class T = char*> T store(const char* key, int value, const char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(const char* key, uint32_t value, const char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(const char* key, double value, const char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(const char* key, float value, const char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(const char* key, long value, const char* timestamp = NULL, bool isAck = true);

		//MSTORE
		template<class T = char**> T mstore(char** keys, char** values, int len, const char* timestamp = NULL, bool isAck = true);

		//STORE TAGS
		ArancinoPacket storetags(const char* key, char** tags, char** values, int len, const char* timestamp = NULL, bool isAck = true);

		//GETRESERVED
		char* getModuleVersion();
		char* getModuleLogLevel();
		char* getModuleEnvironment();
		char* getBlinkId();

		//SETRESERVED
		ArancinoPacket setBlinkId(int value) ;

		/***** DEBUG OPTIONS *****/

		void enableDebugMessages(bool sendViaCommMode=false);
		void enableDebugMessages(Stream& dbgSerial);
		void disableDebugMessages();

		/***** API UTILS *****/
		//FREE
		void free(char* str);
		void free(char** _array);
		void free(ArancinoPacket packet);
		void * calloc(size_t nmemb, size_t _size);
		void * malloc(size_t size);

		//PRINT
		void print(String value);
		void print(const char* value);
		void print(int value);
		void print(float value);
		void print(double value);
		void println(String value);
		void println(int value);
		void println(double value);
		void println(float value);

		//GET ARRAY SIZE
		int getArraySize(char** _array);
		int getArraySize(String* _array);
		//GET ARRAY SIZE
		char* getTimestamp();

		//CHECK UTF-8
		bool isValidUTF8(const char* string);

		//HW CONTROL
		void systemReset();

	private:
		//void dropAll();
		bool started = false;
		bool arancino_id_prefix;
		int decimal_digits;
		char timestamp[13];
		unsigned long timestampMillis;
		unsigned long tmst_sup;
		unsigned long tmst_inf;
		unsigned long millis_previous;

		char LOG_LEVEL[10]="INFO";
		char DAEMON_VER[10];
		char DAEMON_ENV[10];

		//START
		void start(JsonDocument& cmd_doc);

		//API WRAPPED
		void _freeArray(char** _array);
		void _freePacket(ArancinoPacket packet);

		ArancinoPacket __publish(const char* channel, const char* msg, bool isAck = true);
		ArancinoPacket __store(const char* key, const char* value, const char* timestamp=NULL, bool isAck = true);


		template<class T = char*> T getReserved(const char* key);
		ArancinoPacket setReserved( const char* key, const char* value);

		//INTERNAL UTILS
		void _doubleToString(double value, unsigned int _nDecimal, char* str); //truncation!
		void _floatToString(float value, unsigned int _nDecimal, char* str);

		void _printDebugMessage(const char* value);
		
		#if defined(USEFREERTOS)	//The entire prototype would be ill-formed if freertos is not defined
		BaseType_t takeCommMutex(TickType_t timeout);
		#endif
		void giveCommMutex();
		void taskSuspend();
		void taskResume();

		//execute command

		ArancinoPacket createArancinoPacket(const char* response_raw, int response_type);

		ArancinoPacket executeCommand(const char* cmd, const char* key, const char* field, const char* value, bool isAck, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type);
		ArancinoPacket executeCommand(const char* cmd, char** keys, char** fields, char** values, int len, bool isAck, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type);
		ArancinoPacket executeCommand(JsonDocument& cmd_doc, bool isAck, int response_type);
		ArancinoPacket executeCommand(JsonDocument& cmd_doc, JsonDocument& rsp_doc, bool isAck, int response_type);
		ArancinoPacket createArancinoPacket(JsonDocument& response_dict, int response_type);

		void _buildArancinoJson(JsonDocument& doc, const char* cmd, const char* key, const char* field, const char* value, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg);
		void _buildArancinoJson(JsonDocument& doc, const char* cmd, char** keys, char** fields, char** values, int len, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg);

		//Protocol interface
		ArancinoIface* _iface;

		//Debug options
		Stream* _dbgSerial;
		bool _isDebug = false;
		bool _commMode = false;

	friend class ArancinoTasks;
};

extern ArancinoClass Arancino;

#endif /* ARANCINO_H_ */
