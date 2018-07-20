/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : LightingRod.h
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#ifndef LIGHTINGROD_H_
#define LIGHTINGROD_H_

#include <Arduino.h>
#include <Stream.h>

#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define SERIAL_PORT SerialUSB
#endif


#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

class LightingRodClass {
	public:
		void begin();
		void println(String log){};
		String get (String);
		String set (String, String);
		LightingRodClass(Stream &_stream);

	private:
		//void dropAll();
		bool started;
		Stream &stream;
};

// This subclass uses a serial port Stream
class SerialLightingRodClass : public LightingRodClass {
	public:
		SerialLightingRodClass(SERIAL_TRANSPORT &_serial)
			: LightingRodClass(_serial), serial(_serial){
			// Empty	
		}
		void begin( unsigned long baudrate = BAUDRATE) {
			serial.begin(baudrate);
			LightingRodClass::begin();
		}
	private:
		SERIAL_TRANSPORT &serial;
		
};

extern SerialLightingRodClass LRSerial;


#endif /* LIGHTINGROD_H_ */