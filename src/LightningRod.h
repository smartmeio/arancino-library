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
		void println(String log){};
		String get (String);
		String set (String, String);
		LightningRodClass(Stream &_stream);

	private:
		//void dropAll();
		bool started;
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