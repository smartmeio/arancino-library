/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : LightningRod.h
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#ifndef LIGHTNINGROD_H_
#define LIGHTNINGROD_H_

#include <Arduino.h>
#include <Stream.h>

#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define SERIAL_PORT SerialUSB
#endif


#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

class LightningRodClass {
	public:
		void begin();
		String get (String value);
		void set (String key, String value);
		void set (String key, int value);
		void set (String key, double value);
		int del (String key);
		int del (String* key, int number);
		int set(String key, String field, String value);
		String* keys(String pattern="");
		String hget(String key, String field);
		String* hgetall(String key);
		String* hkeys(String key);
		String* hvals(String key);
		int hset( String Key, String field, String value);
		int hdel( String key, String field);
		int hdel( String key, String* fields);
		LightningRodClass(Stream &_stream);
		//TO-DO
		//void parseArray(String message);
		int arraySize=0;
		String* arrayKey;
		
	private:
		//void dropAll();
		//bool started;
		//String* arrayKey;
		String parse(String message);
		String* parseArray(String message);
		Stream &stream;
};

// This subclass uses a serial port Stream
class SerialLightningRodClass : public LightningRodClass {
	public:
		SerialLightningRodClass(SERIAL_TRANSPORT &_serial)
			: LightningRodClass(_serial), serial(_serial){
			// Empty	
		}
		void begin( unsigned long baudrate = BAUDRATE) {
			serial.begin(baudrate);
			LightningRodClass::begin();
		}
	private:
		SERIAL_TRANSPORT &serial;
		
};

extern SerialLightningRodClass LRSerial;


#endif /* LIGHTNINGROD_H_ */