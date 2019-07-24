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

#define USEFREERTOS
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
extern "C" {
#include <FreeRTOS_SAMD21.h>
}
#include <ThreadSafeUART.h>
#endif


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
  int value;
  char* string;
  char** stringArray;
} ArancinoResponse;

typedef struct {
  bool isError;
  int responseCode;
  int responseType;
  ArancinoResponse response;
} ArancinoPacket;


class ArancinoClass {
	public:
		//void begin();
		void begin(int timeout);
        void startScheduler();
		void setReservedCommunicationMode(int mode);
        ArancinoPacket get(char* value);
		ArancinoPacket set(char* key, char* value);
		ArancinoPacket set(char* key, int value);
		ArancinoPacket set(char* key, double value);
		ArancinoPacket set(char* key, uint32_t value);
		ArancinoPacket del(char* key);
		//int del (String* key, int number);
		//int set(String key, String field, String value);
		ArancinoPacket keys(char* pattern="");
		ArancinoPacket hget(char* key, char* field);
		ArancinoPacket hgetall(char* key);
		ArancinoPacket hkeys(char* key);
		ArancinoPacket hvals(char* key);
		ArancinoPacket hset(char* key, char* field, char* value);
		ArancinoPacket hset(char* key, char* field, int value);
		ArancinoPacket hset(char* key, char* field, double value);
		ArancinoPacket hset(char* key, char* field, uint32_t value);
		ArancinoPacket hdel(char* key, char* field);
		ArancinoPacket publish(char* channel, char* msg);
		ArancinoPacket publish(int channel, char* msg);
		ArancinoPacket flush(void);

		void print(String value);
		void print(char* value);
		void print(int value);
		void print(double value);
		void println(String value);
		void println(int value);
		void println(double value);
        int getArraySize(char** _array);
        void freeArray(char** _array);
		//int hdel( String key, String* fields, int number);
		ArancinoClass(Stream &_stream);

	private:
		//void dropAll();
		bool started;
        int getStatus(char* data);
		char* parse(char* message);
		char** parseArray(char* message);
		void sendArancinoCommand(String command);
		void sendArancinoCommand(char* command);
		void sendArancinoCommand(char command);
        char* receiveArancinoResponse(char terminator);
		bool isReservedKey(String key);
        bool isReservedKey(char* key);
		char reservedKey[4][11]; //max 10 char for key
		Stream &stream;
        void doubleToString(double value, unsigned int _nDecimal, char* str); //truncation!
        int getDigit(long value);
        int COMM_MODE = SYNCH;
		void sendViaCOMM_MODE(char* key, char* value);
		ArancinoPacket _set(char* key, char* value);
        ArancinoPacket _publish(char* channel, char* msg);
        const char dataSplitStr[2] = {DATA_SPLIT_CHAR, '\0'};
        const char endTXStr[2] = {END_TX_CHAR, '\0'};
};

// This subclass uses a serial port Stream
class SerialArancinoClass : public ArancinoClass {
	public:
		SerialArancinoClass(SERIAL_TRANSPORT &_serial)
			: ArancinoClass(_serial), serial(_serial){
			// Empty
		}
		//void begin(int timeout=TIMEOUT, unsigned long baudrate = BAUDRATE) {
		void begin(int timeout=TIMEOUT) {
			serial.begin(BAUDRATE);
			ArancinoClass::begin(timeout);
		}
	private:
		SERIAL_TRANSPORT &serial;

};

extern SerialArancinoClass Arancino;


#endif /* ARANCINO_H_ */
