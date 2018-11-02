/*
****************************************************************************
* Copyright (c) 2018 SmartME.IO. All right reserved.
*
* File : Arancino.cpp
* Date : 2018/07/19
* Revision : 0.0.1 $
* Author: andrea[at]smartme[dot]io
*
****************************************************************************

*/

#include "Arancino.h"

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

ArancinoClass::ArancinoClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}

/*void ArancinoClass::begin() {
	begin(TIMEOUT);
}*/

void ArancinoClass::begin(int timeout) {

	String start;
  stream.setTimeout(timeout);			//response timeout
  //DEBUG
  pinMode(DBG_PIN,INPUT);
  if(!digitalRead(DBG_PIN)){
  	Serial.begin(115200);
  }
  // Start communication with serial module on CPU
	do{
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		sendArancinoCommand(START_COMMAND);
		sendArancinoCommand(END_TX_CHAR);				//check if bridge python is running
		start = stream.readStringUntil(END_TX_CHAR);
	}while (start.toInt() != RSP_OK);
}

String ArancinoClass::get( String key ) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(GET_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message);
}

int ArancinoClass::del( String key ) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(DEL_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

/*int ArancinoClass::del( String* key , int number) {

	sendArancinoCommand(DEL_COMMAND);					// send read request
	for(int i=0;i<number;i++){
		if(key[i] != ""){
			sendArancinoCommand(DATA_SPLIT_CHAR);
			sendArancinoCommand(key[i]);
		}
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}*/

void ArancinoClass::set( String key, String value ) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(SET_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (value != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(value);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parse(message);

}

void ArancinoClass::set( String key, int value ) {
	set(key, String(value));
}

void ArancinoClass::set( String key, double value ) {
	set(key, String(value));
}

String ArancinoClass::hget( String key, String field ) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HGET_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (field != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message);
}

String* ArancinoClass::hgetall( String key) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HGETALL_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* ArancinoClass::hkeys( String key) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HKEYS_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* ArancinoClass::hvals( String key) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HVALS_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;
}

String* ArancinoClass::keys(String key){

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(KEYS_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;

};


int ArancinoClass::hset( String key, String field , String value) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HSET_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (field != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	if (value != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(value);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

int ArancinoClass::hdel( String key, String field ) {

	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	sendArancinoCommand(HDEL_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (field != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

/*int ArancinoClass::hdel( String key, String* fields , int number) {

	sendArancinoCommand(HDEL_COMMAND);					// send read request
	if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	for(int i=0;i<number;i++){
		if(fields[i] != ""){
			sendArancinoCommand(DATA_SPLIT_CHAR);
			sendArancinoCommand(fields[i]);
		}
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}*/

void ArancinoClass::parseArray(String data){
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

String ArancinoClass::parse(String message){

	String status;
	String value = "";
	int statusIndex = message.indexOf(DATA_SPLIT_CHAR);
	//int valueIndex = message.indexOf(DATA_SPLIT_CHAR, statusIndex+1);
	status = message.substring(0, statusIndex);				//message status (0: no message; 1:data ready; -1:error)
	if(statusIndex != -1)
		value = message.substring(statusIndex+1);
	//DEBUG
	if(!digitalRead(DBG_PIN)){
		Serial.print(RCV_STRING);
		Serial.print(status);
		Serial.print(" ");
		Serial.println(value);
	}
	return value;

}

void ArancinoClass::sendArancinoCommand(String command){
	stream.print(command);
	if(!digitalRead(DBG_PIN)){
		Serial.print(command);
	}
}

void ArancinoClass::sendArancinoCommand(char command){
	stream.print(command);
	if(!digitalRead(DBG_PIN)){
		if(command == END_TX_CHAR)
			Serial.println(command);
		else
			Serial.print(command);
	}
}

/*void ArancinoClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}*/

// Arancino instance
SerialArancinoClass Arancino(SerialUSB);

//#endif
