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
#define LIB_VERSION				"0.1.1"	//library version

ArancinoClass::ArancinoClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}

/*void ArancinoClass::begin() {
	begin(TIMEOUT);
}*/

//============= SETUP FUNCTIONS ======================

void ArancinoClass::begin(int timeout) {

	String start;
	//reserved Key
	reservedKey[0]=MONITOR_KEY;
  reservedKey[1]=LIBVERS_KEY;
  reservedKey[2]=MODVERS_KEY;
  reservedKey[3]=POWER_KEY;
  stream.setTimeout(timeout);			//response timeout
  //DEBUG
  #if defined(__SAMD21G18A__)
  pinMode(DBG_PIN,INPUT);
  if(!digitalRead(DBG_PIN)){
  	Serial.begin(115200);
  }
  #endif
  // Start communication with serial module on CPU
	do{
		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif
		sendArancinoCommand(START_COMMAND);
		sendArancinoCommand(END_TX_CHAR);				//check if bridge python is running
		start = stream.readStringUntil(END_TX_CHAR);
	}while (start.toInt() != RSP_OK);

	// sendArancinoCommand(SET_COMMAND);					// send library version
	// sendArancinoCommand(DATA_SPLIT_CHAR);
	// sendArancinoCommand(LIBVERS_KEY);
	// sendArancinoCommand(DATA_SPLIT_CHAR);
	// sendArancinoCommand(LIB_VERSION);
	// sendArancinoCommand(END_TX_CHAR);
	// stream.readStringUntil(END_TX_CHAR);

	sendViaCOMM_MODE(LIBVERS_KEY, LIB_VERSION);

}

// void ArancinoClass::setReservedCommunicationMode(int mode){
// 	COMM_MODE = mode;
// }

//============= API FUNCTIONS ======================

String ArancinoClass::get( String key ) {

	if(isReservedKey(key)){
		return "";
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

	if(isReservedKey(key)){
		return -1;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

void ArancinoClass::_set( String key, String value ) {
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

void ArancinoClass::set( String key, String value ) {

	if(isReservedKey(key)){
		//TODO maybe it's better to print a log
		return;
	}
	_set(key, value);

}

void ArancinoClass::set( String key, int value ) {
	set(key, String(value));
}

void ArancinoClass::set( String key, double value ) {
	set(key, String(value));
}

String ArancinoClass::hget( String key, String field ) {

	if(isReservedKey(key)){
		return "";
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

	if(isReservedKey(key)){
		return NULL;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

	if(isReservedKey(key)){
		return NULL;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

	if(isReservedKey(key))
		return NULL;
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

String* ArancinoClass::keys(String pattern){
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(KEYS_COMMAND);					// send read request
	if (pattern != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(pattern);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parseArray(parse(message));
	return arrayKey;

};

int ArancinoClass::hset( String key, String field , String value) {

	if(isReservedKey(key)){
		return -1;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

int ArancinoClass::hset( String key, String field, int value ) {
	hset(key, field, String(value));
}

int ArancinoClass::hset( String key, String field, double value ) {
	hset(key, field, String(value));
}

int ArancinoClass::hdel( String key, String field ) {

	if(isReservedKey(key))
		return -1;

	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
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

int ArancinoClass::_publish( String channel, String msg ) {
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(PUBLISH_COMMAND);					// send read request
	if (channel != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(channel);
	}
	if (msg != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(msg);
	}
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	return parse(message).toInt();
}

int ArancinoClass::publish( String channel, String msg ) {

	if(isReservedKey(channel)){
		//TODO maybe its better to log the error
		return -1;
	}
	return _publish(channel, msg);

}

int ArancinoClass::publish( int channel, String msg ) {
	publish(String(channel), msg);
}

void ArancinoClass::flush() {

	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(FLUSH_COMMAND);					// send read request
	/*if (key != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (value != ""){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(value);
	}*/
	sendArancinoCommand(END_TX_CHAR);
	String message = stream.readStringUntil(END_TX_CHAR);
	parse(message);

}

int ArancinoClass::getArraySize(){
		return arraySize;
}

//============= DEBUG FUNCTIONS ======================

void ArancinoClass::print(String value){
	sendViaCOMM_MODE(MONITOR_KEY, value);
}

void ArancinoClass::print(int value) {
	print(String(value));
}

void ArancinoClass::print(double value) {
	print(String(value));
}

void ArancinoClass::println(String value){
	print(value+String('\n'));
}

void ArancinoClass::println(int value) {
	println(String(value));
}

void ArancinoClass::println(double value) {
	println(String(value));
}

void ArancinoClass::sendViaCOMM_MODE(String key, String value){
	switch (COMM_MODE) {
		case ASYNCH:
			_set(key, value);
		break;

		case SYNCH:
			_publish(key, value);
		break;

		case BOTH:
			_publish(key, value);
			_set(key, value);
		break;

		default:
			_set(key, value);
		break;
	}
}


//=================================================
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
	arraySize=0;					//reset size array
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
	if(statusIndex != -1){
		value = message.substring(statusIndex+1);
	}
	//DEBUG
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(RCV_STRING);
		Serial.print(status);
		Serial.print(" ");
		Serial.println(value);
	}
	#endif
	return value;

}

void ArancinoClass::sendArancinoCommand(String command){
	stream.print(command);
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(command);
	}
	#endif
}

void ArancinoClass::sendArancinoCommand(char command){
	stream.print(command);
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		if(command == END_TX_CHAR)
			Serial.println(command);
		else
			Serial.print(command);
	}
	#endif
}

bool ArancinoClass::isReservedKey(String key){

	for(int i=0;i<sizeof(reservedKey);i++){
		if(reservedKey[i] == key)
			return true;
	}
	return false;

}
/*void ArancinoClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}*/

// Arancino instance
#if defined(__SAMD21G18A__)
SerialArancinoClass Arancino(SerialUSB);
#else
SerialArancinoClass Arancino(Serial);
#endif
//#endif
