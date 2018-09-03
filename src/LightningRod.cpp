/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : LightningRod.cpp
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#include "LightningRod.h"

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

#define END_TX_CHAR			(char)4 //'@' //
#define DATA_SPLIT_CHAR	(char)30 //'#' //
#define TIMEOUT 				100
#define RSP_OK					200
#define	RSP_KO					100
#define	ERR_NOT_AVAIL		101
#define	ERR_SET					102
#define	ERR_NOT_FOUND		103

//String arrayKey[3]={}; 

LightningRodClass::LightningRodClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}

void LightningRodClass::begin() {
	
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

String LightningRodClass::get( String key ) {
	
	stream.print(GET_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return parse(message);
}

int LightningRodClass::del( String key ) {
	
	stream.print(DEL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return parse(message).toInt();
}

int LightningRodClass::del( String* key , int number) {
	
	stream.print(DEL_COMMAND);					// send read request 
	for(int i=0;i<number;i++){
		if(key[i] != ""){
			stream.print(DATA_SPLIT_CHAR);
			stream.print(key[i]);
		}		
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return parse(message).toInt();
}

void LightningRodClass::set( String key, String value ) {

	stream.print(SET_COMMAND);					// send read request 
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
	//return parse(message).toInt();
	
}

void LightningRodClass::set( String key, int value ) {
	set(key, String(value)); 	
}

void LightningRodClass::set( String key, double value ) {
	set(key, String(value)); 	
}

String LightningRodClass::hget( String key, String field ) {
	
	stream.print(HGET_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	if (field != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(field);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return parse(message);
}

String* LightningRodClass::hgetall( String key) {
	
	stream.print(HGETALL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::hkeys( String key) {
	
	stream.print(HKEYS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::hvals( String key) {
	
	stream.print(HVALS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::keys(String key){
	
	stream.print(KEYS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
	
};


int LightningRodClass::hset( String key, String field , String value) {
	
	stream.print(HSET_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	if (field != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(field);
	}
	if (value != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(value);
	}
	stream.println();
	String message = stream.readStringUntil(END_TX_CHAR);
	//data = parse(message, ";");
	return parse(message).toInt();
}

void LightningRodClass::parseArray(String data){
	arraySize=0;																	//reset size array
  int idx=data.indexOf(DATA_SPLIT_CHAR);
  while(idx!=-1){
    idx=data.indexOf(DATA_SPLIT_CHAR,idx+1);		//split char recurrence in a command
    arraySize++;
  }
  arraySize++;
  if (arrayKey != 0) {
    delete [] arrayKey;										//free size
	}
	arrayKey = new String[arraySize];
	idx=0;
	int idx_temp=0;
	int i=0;
	while(idx_temp!=-1){
	  idx_temp=data.indexOf(DATA_SPLIT_CHAR,idx+1);
		arrayKey[i++]=data.substring(idx,idx_temp);
		idx=idx_temp+1;
	}
  //return arrayKey;
}

String LightningRodClass::parse(String message){
	
	String status;
	String value;
	int statusIndex = message.indexOf(DATA_SPLIT_CHAR);
	int valueIndex = message.indexOf(DATA_SPLIT_CHAR, statusIndex+1);
	status = message.substring(0, statusIndex);				//message status (0: no message; 1:data ready; -1:error)
	value = message.substring(statusIndex+1);
	return value;
	
}

/*void LightningRodClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}*/

// LightningRod instance
SerialLightningRodClass LRSerial(SerialUSB);

//#endif