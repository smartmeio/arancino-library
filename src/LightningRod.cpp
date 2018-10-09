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

LightningRodClass::LightningRodClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}

/*void LightningRodClass::begin() {
	begin(TIMEOUT);
}*/

void LightningRodClass::begin(int timeout) {
	
	String start;
  stream.setTimeout(timeout);			//response timeout
  // Start communication with serial module on CPU
	do{
		stream.print(START_COMMAND); 
		stream.print(END_TX_CHAR);				//check if bridge python is running
		start = stream.readStringUntil(END_TX_CHAR);
	}while (start.toInt() != RSP_OK);
}

String LightningRodClass::get( String key ) {
	
	stream.print(GET_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message);
}

int LightningRodClass::del( String key ) {
	
	stream.print(DEL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

/*int LightningRodClass::del( String* key , int number) {
	
	stream.print(DEL_COMMAND);					// send read request 
	for(int i=0;i<number;i++){
		if(key[i] != ""){
			stream.print(DATA_SPLIT_CHAR);
			stream.print(key[i]);
		}		
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}*/

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
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	
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
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message);
}

String* LightningRodClass::hgetall( String key) {
	
	stream.print(HGETALL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::hgetkeys( String key) {
	
	stream.print(HKEYS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::hvals( String key) {
	
	stream.print(HVALS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* LightningRodClass::keys(String key){
	
	stream.print(KEYS_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	stream.print(END_TX_CHAR);
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
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

int LightningRodClass::hdel( String key, String field ) {
	
	stream.print(HDEL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	if (field != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(field);
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

/*int LightningRodClass::hdel( String key, String* fields , int number) {
	
	stream.print(HDEL_COMMAND);					// send read request 
	if (key != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(key);
	}
	for(int i=0;i<number;i++){
		if(fields[i] != ""){
			stream.print(DATA_SPLIT_CHAR);
			stream.print(fields[i]);
		}		
	}
	stream.print(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}*/

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
}

String LightningRodClass::parse(String message){
	
	String status;
	String value = "";
	int statusIndex = message.indexOf(DATA_SPLIT_CHAR);
	//int valueIndex = message.indexOf(DATA_SPLIT_CHAR, statusIndex+1);
	status = message.substring(0, statusIndex);				//message status (0: no message; 1:data ready; -1:error)
	if(statusIndex != -1)
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