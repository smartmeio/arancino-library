/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : Arancino.h
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#ifndef ARANCINO_H_
#define ARANCINO_H_

#include <Arduino.h>
#include <Stream.h>

#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define TIMEOUT 100
#define SERIAL_PORT SerialUSB
#endif


#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

class ArancinoClass {
	public:
		//void begin();
		void begin(int timeout);
		String get(String value);
		void set(String key, String value);
		void set(String key, int value);
		void set(String key, double value);
		int del(String key);
		//int del (String* key, int number);
		int set(String key, String field, String value);
		String* keys(String pattern="");
		String hget(String key, String field);
		String* hgetall(String key);
		String* hkeys(String key);
		String* hvals(String key);
		int hset(String Key, String field, String value);
		int hset(String key, String field, int value);
		int hset(String key, String field, double value);
		int hdel(String key, String field);
		
		void print(String value);
		void print(int value);
		void print(double value);
		void println(String value);
		void println(int value);
		void println(double value);
		//int hdel( String key, String* fields, int number);
		ArancinoClass(Stream &_stream);
		int arraySize=0;
		String* arrayKey;

	private:
		//void dropAll();
		bool started;
		String parse(String message);
		void parseArray(String message);
		void sendArancinoCommand(String command);
		void sendArancinoCommand(char command);
		bool checkReservedKey(String key);
		Stream &stream;
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
