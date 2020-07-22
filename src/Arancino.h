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

//

class ArancinoClass {
	public:
		//void begin();
		String id;
		void begin(int timeout = TIMEOUT, bool useid = false);
		void setReservedCommunicationMode(int mode);
		String get(String value);
		void set(String key, String value);
		void set(String key, int value);
		void set(String key, double value);
		void set( String key, uint32_t value);
		void set( String key, long value);
		int del(String key);
		//int del (String* key, int number);
		int set(String key, String field, String value);
		String* keys(String pattern="");
		String hget(String key, String field);
		String* hgetall(String key);
		String* hkeys(String key);
		String* hvals(String key);
		int hset(String key, String field, String value);
		int hset(String key, String field, int value);
		int hset(String key, String field, double value);
		int hset(String key, String field, uint32_t value);
		int hset(String key, String field, long value);
		int hdel(String key, String field);
		int publish(String channel, String msg);
		//int publish(int channel, String msg);
		int publish(String channel, int msg);
		int publish(String channel, float msg);
		int publish(String channel, long msg);
		int publish(String channel, uint32_t msg);
		void flush(void);

		void print(String value);
		void print(int value);
		void print(double value);
		void println(String value);
		void println(int value);
		void println(double value);
		int getArraySize();
		//int hdel( String key, String* fields, int number);
		ArancinoClass(Stream &_stream);
		String* arrayKey;
		//int arraySize=0;

	private:
		//void dropAll();
		bool arancino_id_prefix;
		String timestamp;

		bool started;
		bool comm_timeout = false;
		String parse(String message);
		void parseArray(String message);
		void sendArancinoCommand(String command);
		void sendArancinoCommand(char command);
		bool isReservedKey(String key);
		String reservedKey[RESERVED_KEY_ARRAY_SIZE];
		Stream &stream;
		int arraySize=0;
		int COMM_MODE = SYNCH;
		void sendViaCOMM_MODE(String key, String value);
		void _set(String key, String value);
		int _publish(String channel, String msg);
		void flush_on_timeout();
		String receiveArancinoResponse(char terminator);
};

// This subclass uses a serial port Stream
class SerialArancinoClass : public ArancinoClass {
	public:
		SerialArancinoClass(SERIAL_TRANSPORT &_serial)
			: ArancinoClass(_serial), serial(_serial){
			// Empty
		}
		//void begin(int timeout=TIMEOUT, unsigned long baudrate = BAUDRATE) {
		void begin(int timeout = TIMEOUT, bool useid = false) {
			serial.begin(BAUDRATE);
			ArancinoClass::begin(timeout, useid);
		}
	private:
		SERIAL_TRANSPORT &serial;

};

extern SerialArancinoClass Arancino;


#endif /* ARANCINO_H_ */
