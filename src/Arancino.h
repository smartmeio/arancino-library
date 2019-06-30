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
#include <Stream.h>
#include <stdlib.h>


#define USEFREERTOS
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
extern "C" {
#include <FreeRTOS_SAMD21.h>
}
#endif

#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define TIMEOUT 100
#define SERIAL_PORT SerialUSB
#else
#define BAUDRATE 256000
#define TIMEOUT 100
#define SERIAL_PORT Serial
#endif

#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

#define START_COMMAND 		"START"
#define SET_COMMAND 			"SET"
#define GET_COMMAND 			"GET"
#define DEL_COMMAND 			"DEL"
#define KEYS_COMMAND			"KEYS"
#define HGET_COMMAND			"HGET"
#define HGETALL_COMMAND		"HGETALL"
#define HKEYS_COMMAND			"HKEYS"
#define HDEL_COMMAND			"HDEL"
#define HSET_COMMAND			"HSET"
#define HVALS_COMMAND			"HVALS"
#define PUBLISH_COMMAND		"PUB"
#define FLUSH_COMMAND			"FLUSH"

#define SENT_STRING				"Sent Command: "
#define RCV_STRING				"Received Response: "

#define END_TX_CHAR				(char)4 //'@' //
#define DATA_SPLIT_CHAR		(char)30 //'#' //

#define RSP_OK						100
#define RSP_HSET_NEW			101
#define RSP_HSET_UPD			102
#define ERR								200		//Generic Error
#define ERR_NULL					201		//Null value
#define ERR_SET						202		//Error during SET
#define ERR_CMD_NOT_FND		203		//Command Not Found
#define ERR_CMD_NOT_RCV		204		//Command Not Received
#define ERR_CMD_PRM_NUM		205		//Invalid parameter number
#define ERR_REDIS					206		//Generic Redis Error

#define DBG_PIN						26		//pin used to Debug Message
//#define PWR_PIN					??		//pin used for Power Management

#define MONITOR_KEY				"___MONITOR___"
#define LIBVERS_KEY				"___LIBVERS___"
#define MODVERS_KEY				"___MODVERS___"
#define POWER_KEY					"___POWER___"
#define LIB_VERSION				"0.1.2"	//library version

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

//

class ArancinoClass {
	public:
		//void begin();
		void begin(int timeout);
        void startScheduler();
		void setReservedCommunicationMode(int mode);
        char* get(char* value);
		void set(char* key, char* value);
		void set(char* key, int value);
		void set(char* key, double value); //TODO - BUG
		void set(char* key, uint32_t value);
		int del(char* key);
		//int del (String* key, int number);
		//int set(String key, String field, String value);
		char** keys(char* pattern="");
		char* hget(char* key, char* field);
		char** hgetall(char* key);
		char** hkeys(char* key);
		char** hvals(char* key);
		int hset(char* key, char* field, char* value);
		int hset(char* key, char* field, int value);
		int hset(char* key, char* field, double value); //TODO - BUG
		int hset(char* key, char* field, uint32_t value);
		int hdel(char* key, char* field);
		int publish(char* channel, char* msg);
		int publish(int channel, char* msg);
		void flush(void);

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
		int arraySize=0;
		int COMM_MODE = SYNCH;
		void sendViaCOMM_MODE(char* key, char* value);
		void _set(char* key, char* value);
        int _publish(char* channel, char* msg);
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
