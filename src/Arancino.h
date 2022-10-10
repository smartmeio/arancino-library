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
#include <avr/dtostrf.h>

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
	char* fwname;
	char* fwversion;
	char* tzoffset;
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
		void delay(long milli);

		/***** API BASIC *****/

		//ATTACH_INTERFACE
		void attachInterface(ArancinoIface& iface);

		//BEGIN
		void begin(ArancinoMetadata amdata, ArancinoConfig aconfig);
		void begin(ArancinoMetadata amdata);

		ArancinoPacket set(char* key, int value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(char* key, double value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(char* key, float value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(char* key, long value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(char* key, uint32_t value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket set(char* key, char* value, bool isAck = true, bool isPersistent = false, const char* type = "appl");

		//MSET
		ArancinoPacket mset(char** keys, char** values, int len, bool isAck, bool isPersistent = false, const char* type = "appl");

		//GET
		template<class T = char*> T get(char* key, bool isPersistent = false, const char* type = "appl");

		//MGET
		template<class T = char**> T mget(char** keys, int len, bool isPersistent = false, const char* type = "appl");

		//DEL
		template<class T = int> T del(char* key, bool isAck = true, bool isPersistent = false, const char* type = "appl");

		//HSET
		ArancinoPacket hset(char* key, char* field, char* value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(char* key, char* field, int value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(char* key, char* field, float value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(char* key, char* field, double value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(char* key, char* field, uint32_t value, bool isAck = true, bool isPersistent = false, const char* type = "appl");
		ArancinoPacket hset(char* key, char* field, long value, bool isAck = true, bool isPersistent = false, const char* type = "appl";

		//HGET
		template<class T = char*> T hget(char* key, char* field, bool isPersistent = true, const char* type = "appl");

		//HDEL
		template<class T = int> T hdel(char* key, char* field, bool isAck = true, bool isPersistent = false);

		//PUBLISH
		template<class T = int> T publish(char* channel, char* msg, bool isAck = true);
		template<class T = int> T publish(char* channel, double msg, bool isAck = true);
		template<class T = int> T publish(char* channel, int msg, bool isAck = true);
		template<class T = int> T publish(char* channel, uint32_t msg, bool isAck = true);
		template<class T = int> T publish(char* channel, long msg, bool isAck = true);

		//FLUSH
		ArancinoPacket flush(bool isAck = true, bool isPersistent = false);

		//STORE
		template<class T = char*> T store(char* key, int value, bool isAck = true, char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(char* key, uint32_t value, bool isAck = true, char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(char* key, double value, bool isAck = true, char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(char* key, float value, bool isAck = true, char* timestamp = NULL, bool isAck = true);
		template<class T = char*> T store(char* key, long value, bool isAck = true, char* timestamp = NULL, bool isAck = true);

		//MSTORE
		template<class T = char**> T mstore(char** keys, char** values, int len, char* timestamp = NULL, bool isAck = true);

		//STORE TAGS
		ArancinoPacket storetags(char* key, char** tags, char** values, int len, char* timestamp = NULL, bool isAck = true);

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
		void print(char* value);
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
		bool isValidUTF8(const char * string);

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
		const char dataSplitStr[2] = {DATA_SPLIT_CHAR, '\0'};
		const char arraySplitStr[2] = {ARRAY_SPLIT_CHAR, '\0'};
		const char endTXStr[2] = {END_TX_CHAR, '\0'};
		const char nullStr[2] = {NULL_CHAR, '\0'};

		ArancinoMetadata _metadata = {
			(char*)"",
			(char*)"",
			(char*)"+0000"
		};

		//START
		void start(char** keys, char** values, int len);

		//API WRAPPED
		void _freeArray(char** _array);
		void _freePacket(ArancinoPacket packet);

		ArancinoPacket __set(char* key, char* value, bool isPersistent);
		ArancinoPacket __publish(char* channel, char* msg, bool isAck = true);
		ArancinoPacket __store(char* key, char* value, char* timestamp=NULL, bool isAck = true);


		template<class T = char*> T getReserved(char* key, bool id_prefix);
		ArancinoPacket setReserved( char* key, char* value, bool id_prefix);

		//INTERNAL UTILS
		void _doubleToString(double value, unsigned int _nDecimal, char* str); //truncation!
		void _floatToString(float value, unsigned int _nDecimal, char* str);
		int _getDigit(long value);

		void _printDebugMessage(char* value);

		int _getResponseCode(char* data);

		char* _parse(char* message);
		char** _parseArray(char* message);
		
		BaseType_t takeCommMutex(TickType_t timeout);
		void giveCommMutex();
		void taskSuspend();
		void taskResume();

		//execute command

		ArancinoPacket createArancinoPacket(char* response_raw, int response_type);

		ArancinoPacket executeCommand(const char* cmd, char* key, char* field, const char* value, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type);
		ArancinoPacket executeCommand(const char* cmd, char** keys, char** fields, char** values, int len, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type);
		ArancinoPacket executeCommand(JsonDocument& cmd_doc, int response_type);
		ArancinoPacket createArancinoPacket(JsonDocument& response_dict, int response_type);


		void _buildArancinoJson(JsonDocument& doc, const char* cmd, const char* key, const char* field, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg);
		void _buildArancinoJson(JsonDocument& doc, const char* cmd, const char** keys, const char** fields, int len, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg);


		//Protocol interface
		ArancinoIface* _iface;

		//Debug options
		Stream* _dbgSerial;
		bool _isDebug = false;
		bool _commMode = false;

		//TEMPLATE WRAPPED
		// ArancinoPacket _getPacket(char* key);
		// char* _get(char* key);

		// ArancinoPacket _delPacket(char* key);
		// int _del(char* key);

		// ArancinoPacket _hgetPacket(char* key, char* field);
		// char* _hget(char* key, char* field);

		// ArancinoPacket _hgetallPacket(char* key);
		// char** _hgetall(char* key);

		// ArancinoPacket _hkeysPacket(char* key);
		// char** _hkeys(char* key);

		// ArancinoPacket _hvalsPacket(char* key);
		// char** _hvals(char* key);

		// ArancinoPacket _hdelPacket(char* key, char* field);
		// int _hdel(char* key, char* field);

		// ArancinoPacket _keysPacket(char* pattern="");
		// char** _keys(char* pattern="");

		// ArancinoPacket _publishPacket(int channel, char* msg);
		// ArancinoPacket _publishPacket(char* channel, char* msg);
		// int _publish(int channel, char* msg);
		// int _publish(char* channel, char* msg);

};

extern ArancinoClass Arancino;

#endif /* ARANCINO_H_ */
