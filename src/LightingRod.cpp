/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : LightingRod.cpp
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#include "LightingRod.h"

#define SET_METHOD 			"SET"
#define GET_METHOD 			"GET"
#define END_TX_CHAR			(char)4
#define DATA_SPLIT_CHAR	(char)30
#define TIMEOUT 				2000


LightingRodClass::LightingRodClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}

void LightingRodClass::begin() {
	
	//String start;
	//String stop;
  stream.setTimeout(TIMEOUT);			//response timeout
  // Wait for U-boot to finish startup
	/*do {
		dropAll();
		delay(1000);
	}while (stream.available() > 0);*/

	/*do{ 
		stream.println("");				//check if bridge python is running
		start = stream.readStringUntil(END_TX_CHAR);
	}while (start != "VERSION#");*/
}

String LightingRodClass::get( String key ) {
	
	stream.print(GET_METHOD);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return message;
}

String LightingRodClass::set( String key, String value ) {

	stream.print(SET_METHOD);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	if (value != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(value);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return message;
	
}

/*void LightingRodClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}*/

// LightingRod instance
SerialLightingRodClass LRSerial(SerialUSB);

//#endif