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
#include <ArancinoConfig.h>
#include <Stream.h>
#include <stdlib.h>
#include <cstdlib>
#include <type_traits>

//#define USEFREERTOS
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	
	// #if !defined(__MEM_WRAP__)
	// 	#error You are using Arancino Library, please select Menu -> Tools -> Using Arancino Library?: Yes
	// #endif

extern "C" {
#include <FreeRTOS_SAMD21.h>
}
#endif

	



//RESERVED KEYS ARRAY DEF
#define RESERVED_KEY_ARRAY_SIZE 4   // Array dimension of Reserved Keys

/*
Reserved keys communication mode
0 -> SYNC MODE (Default)
1 -> ASYNC MODE
2 -> BOTH (SYNC and ASYNC)

to redifine the value in user space, use #define RSVD_COMM <new value> before #include <Arancino.h>
*/
// #ifndef RSVD_COMM
// #define RSVD_COMM 0
// #endif

//Reserverd Communication Mode
enum RSVD_COMM_MODE {
	SYNCH = 0,
	ASYNCH = 1,
	BOTH = 2
};

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

class ArancinoClass {
	public:
		/***** API ADVANCED *****/

		//Serial port id
		char *id;
		
		//START SCHEDULER
		void startScheduler();
		
		//SET RESERVED COMMUNICATION MODE
		void setReservedCommunicationMode(int mode);

		
		/***** API BASIC *****/
		//METADATA
		void metadata(ArancinoMetadata meta);

		//BEGIN
		void begin(bool useid = false, int timeout = TIMEOUT);

		//MSET
		ArancinoPacket mset(char** keys, char** values, uint len);

		//SET	
		// ArancinoPacket set(char* key, int value);
		// ArancinoPacket set(char* key, double value);
		// ArancinoPacket set(char* key, uint32_t value);
		// ArancinoPacket set(char* key, char* value);

		ArancinoPacket set(char* key, int value, bool isPersistent = false);
		ArancinoPacket set(char* key, double value, bool isPersistent = false);
		ArancinoPacket set(char* key, long value, bool isPersistent = false);
		ArancinoPacket set(char* key, char* value, bool isPersistent = false); 
		ArancinoPacket set(char* key, uint32_t value, bool isPersistent = false); 
		
		//GET
		//ArancinoPacket getPacket(char* key);
		//char* get(char* key);
		//GET W/ TEMPLATE
		template<class T = char*> T get(char* key);


		//DEL
		// ArancinoPacket delPacket(char* key);
		// int del(char* key);
		//DEL W/ TEMPLATE
		template<class T = int> T del(char* key);

		//HSET
		ArancinoPacket hset(char* key, char* field, char* value);
		ArancinoPacket hset(char* key, char* field, int value);
		ArancinoPacket hset(char* key, char* field, double value);
		ArancinoPacket hset(char* key, char* field, uint32_t value);
		ArancinoPacket hset(char* key, char* field, long value);

		//MGET
		template<class T = char**> T mget(char** keys, uint len);
		
		//HGET
		// ArancinoPacket hgetPacket(char* key, char* field);
		// char* hget(char* key, char* field);
		//HGET W TEMPALTE
		template<class T = char*> T hget(char* key, char* field);

		//HGETALL
		//ArancinoPacket hgetallPacket(char* key);
		//char** hgetall(char* key);
		//HGETALL W TEMPALTE
		template<class T = char**> T hgetall(char* key);

		//HKEYS
		// ArancinoPacket hkeysPacket(char* key);
		// char** hkeys(char* key);
		//HKEYS W TEMPALTE
		template<class T = char**> T hkeys(char* key);

		//HVALS
		// ArancinoPacket hvalsPacket(char* key);
		// char** hvals(char* key);
		//HVALS W TEMPALTE
		template<class T = char**> T hvals(char* key);

		//HDEL
		// ArancinoPacket hdelPacket(char* key, char* field);
		// int hdel(char* key, char* field);
		//HDEL W TEMPALTE
		template<class T = int> T hdel(char* key, char* field);

		//KEYS
		// ArancinoPacket keysPacket(char* pattern="");
		// char** keys(char* pattern="");
		//KEYS W TEMPALTE
		template<class T = char**> T keys(char* pattern="*");

		//PUBLISH		
		ArancinoPacket publish(char* channel, char* msg);
		ArancinoPacket publish(char* channel, double msg);
		ArancinoPacket publish(char* channel, int msg);
		ArancinoPacket publish(char* channel, uint32_t msg);
		ArancinoPacket publish(char* channel, long msg);
		
		//FLUSH
		ArancinoPacket flush(void);

		/***** API UTILS *****/
		//FREE
		void free(char* str);
		void free(char** _array);
		void free(ArancinoPacket packet);

		//PRINT
		void print(String value);
		void print(char* value);
		void print(int value);
		void print(double value);
		void println(String value);
		void println(int value);
		void println(double value);

		//GET ARRAY SIZE
		int getArraySize(char** _array);
		int getArraySize(String* _array);

		//CHECK UTF-8
		bool isValidUTF8(const char * string);

		//TEMPLATE TEST
		//template<class T = char*> T ArancinoGet(char* key);


	private:
		//void dropAll();

		bool started;
		bool comm_timeout = false;
		bool arancino_id_prefix;
		int idSize;
		char *timestamp;
		
		char reservedKey[RESERVED_KEY_ARRAY_SIZE][RESERVED_KEY_MAX_LENGTH]; //max 10 char for key
		int COMM_MODE = SYNCH;
		const char dataSplitStr[2] = {DATA_SPLIT_CHAR, '\0'};
		const char arraySplitStr[2] = {ARRAY_SPLIT_CHAR, '\0'};
		const char endTXStr[2] = {END_TX_CHAR, '\0'};
		const char nullStr[2] = {NULL_CHAR, '\0'};

		ArancinoMetadata _meta = {
			"",
			"",
			"+0000"
		};

		//API WRAPPED
		void _freeArray(char** _array);
		void _freePacket(ArancinoPacket packet);
		
		ArancinoPacket __set(char* key, char* value, bool isPersistent);
		ArancinoPacket __publish(char* channel, char* msg);

		//INTERNAL UTILS
		//void _sendArancinoCommand(String command);
		void _sendArancinoCommand(char* command);
		void _sendArancinoCommand(char command);

		char* _receiveArancinoResponse(char terminator);
		
		//bool _isReservedKey(String key);
		bool _isReservedKey(char* key);
		void _doubleToString(double value, unsigned int _nDecimal, char* str); //truncation!
		int _getDigit(long value);
		
		// ArancinoPacket _sendViaCOMM_MODE(char* key, char* value);
		ArancinoPacket _sendViaCOMM_MODE(char* key, char* value, bool isPersistent = false);

		int _getResponseCode(char* data);
		
		char* _parse(char* message);
		char** _parseArray(char* message);

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
