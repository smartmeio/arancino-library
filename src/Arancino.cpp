/*
SPDX-license-identifier: Apache-2.0

Copyright (C) 2018 SmartMe.IO

Authors:  Andrea CannistrÃ  <andrea@smartme.io>

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
#include <avr/dtostrf.h>

#define DEBUG 0

ArancinoPacket reservedKeyErrorPacket = {true, RESERVED_KEY_ERROR, RESERVED_KEY_ERROR, {.string = NULL}}; //default reserved key error packet
ArancinoPacket communicationErrorPacket = {true, COMMUNICATION_ERROR, COMMUNICATION_ERROR, {.string = NULL}}; //default reserved key error packet
ArancinoPacket invalidCommandErrorPacket = {true, INVALID_VALUE_ERROR, INVALID_VALUE_ERROR, {.string = NULL}}; //default reserved key error packet

//TEMPLATE TEST
// template<> ArancinoPacket ArancinoClass::ArancinoGet<ArancinoPacket> (char* key){
// 	ArancinoPacket result = {true, -1, -1, {.string = "test"}};
// 	return result;
// }

// template<> char* ArancinoClass::ArancinoGet (char* key){
// 	//DEFAULT

// 	return "value";
// }

/*ArancinoClass::ArancinoClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}*/

/*void ArancinoClass::begin() {
	begin(TIMEOUT);
}*/

/********************************************************
					API FUNCTIONS
********************************************************/

/******** API BASIC :: METADATA *********/

[[deprecated("Pass ArancinoMetadata to the Arancino.begin function.")]]
void ArancinoClass::metadata(ArancinoMetadata _amdata) {
	_metadata = _amdata;
}

/******** API BASIC :: BEGIN *********/

// void ArancinoClass::begin(ArancinoMetadata _amdata, bool useid, int timeout) {
// 	ArancinoConfig _acfg;
// 	_acfg._USEID = useid;
// 	_acfg._TIMEOUT = timeout;
// 	begin(_amdata, _acfg);
// }


void ArancinoClass::begin(ArancinoMetadata _amdata, bool useid, int timeout) {
	ArancinoConfig _acfg;
	_acfg._USEID = useid;
	_acfg._TIMEOUT = timeout;
	begin(_amdata, _acfg);
}

void ArancinoClass::begin(ArancinoMetadata _amdata, ArancinoConfig _acfg) {
	SERIAL_PORT.begin(BAUDRATE);
	// SERIAL_PORT.setTimeout(TIMEOUT);
	// arancino_id_prefix = false;
	arancino_id_prefix = _acfg.USE_PORT_ID_PREFIX_KEY;
	SERIAL_PORT.setTimeout(_acfg.SERIAL_TIMEOUT);
	decimal_digits=_acfg.DECIMAL_DIGITS;

	_metadata = _amdata;

	int fwnameLength = strlen(_metadata.fwname);
	int fwversionLength = strlen(_metadata.fwversion);
	int localtoffsetLength = strlen(_metadata.tzoffset);

	int dateLength = strlen(__DATE__);
	int timeLength = strlen(__TIME__);
	int arancinocoreversionLength = 0; // assuming is not defined

	#ifdef ARANCINO_CORE_VERSION
	arancinocoreversionLength += strlen(ARANCINO_CORE_VERSION);
	#endif

	int commandLength = strlen(START_COMMAND);
	int argLength = strlen(LIB_VERSION);
	int strLength = commandLength + 1 + argLength + 1 + fwnameLength + 1 + fwversionLength + 1 + dateLength + 1 + timeLength + 1 + localtoffsetLength + arancinocoreversionLength + 1 + 1;

	char* str = (char *)calloc(strLength, sizeof(char));

	//reserved Key
	strcpy(reservedKey[0], MONITOR_KEY);
	strcpy(reservedKey[1], LIBVERS_KEY);
	strcpy(reservedKey[2], MODVERS_KEY);
	strcpy(reservedKey[3], POWER_KEY);
	//DEBUG
	#if defined(__SAMD21G18A__)
	pinMode(DBG_PIN,INPUT);
	if(!digitalRead(DBG_PIN)){
		Serial.begin(115200);
	}
	#endif

	//RTC
	strcpy(str, START_COMMAND);
	strcat(str, dataSplitStr);

	strcat(str, LIB_VERSION);
	strcat(str, dataSplitStr);

	strcat(str, _metadata.fwname);
	strcat(str, dataSplitStr);
	strcat(str, _metadata.fwversion);
	strcat(str, dataSplitStr);

	strcat(str, __DATE__);
	strcat(str, " ");
	strcat(str, __TIME__);
	strcat(str, " ");
	strcat(str, _metadata.tzoffset);
	strcat(str, dataSplitStr);

	#ifdef ARANCINO_CORE_VERSION
	strcat(str, ARANCINO_CORE_VERSION);
	#endif

	strcat(str, endTXStr);

	ArancinoPacket packet;
	// Start communication with serial module on CPU
	do{
		//try to start comunication every 2,5 seconds.
		delay(2500);

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif


		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);

		if (message != NULL)
		{
			//ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}};
			ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};

			packet = temp;

			if(packet.responseCode == RSP_OK){
				//store arancino serial port id
				idSize = strlen(packet.response.stringArray[0]);
				id = (char *)calloc(idSize+1, sizeof(char));
				memcpy(id,packet.response.stringArray[0],idSize);
				//timestamp from arancino module
				memcpy(timestamp, packet.response.stringArray[1],13);
				char tmst_part[5];
				memcpy(tmst_part,timestamp, 4);  //first 4 digits of epoch
				tmst_sup=atoi(tmst_part);
				tmst_inf=atoi(&timestamp[4]);  //last 9 digits of epoch
				millis_previous=millis();

			}

			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}

	}while (packet.isError == true || packet.responseCode != RSP_OK);
	std::free(str);

	//lib version is sent via start command.
	//_sendViaCOMM_MODE(LIBVERS_KEY, LIB_VERSION);

}


#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
TaskHandle_t commTaskHandle;

/******** API ADVANCED :: START SCHEDULER *********/

void ArancinoClass::startScheduler() {
	vSetErrorLed(LED_BUILTIN, HIGH);

	/*
	 * Uncomment this if you want run loop() as a dedicated task.
	 * If loop() doesn't run as dedicated task, should not contain blocking code.
	 */
	//runLoopAsTask(256, tskIDLE_PRIORITY);

	vTaskStartScheduler();
}
#endif

/******** API ADVANCED :: SET RESERVED COMMUNICATION MODE *********/

void ArancinoClass::setReservedCommunicationMode(int mode){
	COMM_MODE = mode;
}

/******** API BASIC :: MSET *********/

ArancinoPacket ArancinoClass::mset(char** keys, char** values, uint len) {
	if ((keys == NULL) || (values == NULL) || (len <= 0)) {
		return invalidCommandErrorPacket;
	}

	uint commandLength = strlen(MSET_COMMAND);
	uint strLength = commandLength + 1; // Counting the # character (data split chr)

	// Calculating Cortex Protocol command length
	for (uint i = 0; i < len; i ++) {
		char* key = keys[i];
		char* value = values[i];

		uint keyLength = strlen(key);

		// if arancino_id has to be prepended we take into account its length and _ char
		if (arancino_id_prefix) {
			keyLength += idSize + 1;
		}

		uint valueLength = strlen(value);


		// For every key-value pair the length of both key and value is added
		// two 1s are added in order to take into account the % chr and
		// the # and @ chrs in the case of last key or last value respectively
		strLength += keyLength + 1 + valueLength + 1;
	}

	char* str = (char*) calloc(strLength + 1, sizeof(char));
	strcat(str, MSET_COMMAND);
	strcat(str, dataSplitStr);

	// Points to the memory area where keys have to be written
	char* keysPointer = str + strlen(MSET_COMMAND) + strlen(dataSplitStr);

	// Points at the end of the string
	char* valuesPointer = str + strLength;

	// The string to send is built in 1 single loop.
	// keys are copied from first to last (left to right in string)
	// and values are copied from last to first (right to left in string)
	for(uint i = 0; i < len; i ++) {
		char* key = keys[i];
		char* value = values[len - (i + 1)];

		if(arancino_id_prefix){
			strcat(keysPointer, id);
			strcat(keysPointer, ID_SEPARATOR);
		}

		strcat(keysPointer, key);

		if (i == len - 1) { // If it's the last key we have to use #(\4) instead of %(\16)
			strcat(keysPointer, dataSplitStr);
		} else {
			strcat(keysPointer, arraySplitStr);
		}

		// We use memcpy rather than strcat here because it would append \0,
		// thus terminating the string prematurely
		valuesPointer -= strlen(value) + 1;

		memcpy(valuesPointer, value, strlen(value));

		if (i == 0) {
			memcpy(valuesPointer + strlen(value), endTXStr, 1);
		} else {
			memcpy(valuesPointer + strlen(value), arraySplitStr, 1);
		}
	}

	#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
	#endif

	#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
	#endif

	_sendArancinoCommand(str);
	char* message = _receiveArancinoResponse(END_TX_CHAR);
	#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
	#endif

	std::free(str);


	if (message == NULL) return communicationErrorPacket;

	ArancinoPacket packet = {false, _getResponseCode(message), VOID, {.string = NULL}};
	std::free(message);

	return packet;
}

/******** API BASIC :: MGET *********/

template<> ArancinoPacket ArancinoClass::mget<ArancinoPacket>(char** keys, uint len){
	if ((keys == NULL) || (len <= 0)) {
		return invalidCommandErrorPacket;
	}

	uint commandLength = strlen(MSET_COMMAND);
	uint strLength = commandLength + 1; // Counting the # character (data split chr)

	// Calculating Cortex Protocol command length
	for (uint i = 0; i < len; i ++) {
		char* key = keys[i];

		uint keyLength = strlen(key);

		// if arancino_id has to be prepended we take into account its length and _ char
		if (arancino_id_prefix) {
			keyLength += idSize + 1;
		}

		strLength += keyLength + 1;
	}

	char* str = (char*) calloc(strLength, sizeof(char));
	strcat(str, MGET_COMMAND);
	strcat(str, dataSplitStr);


	// Building the string
	for(uint i = 0; i < len; i ++) {
		char* key = keys[i];

		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}

		strcat(str, key);
		if (i != len - 1) strcat(str, arraySplitStr);
	}

	strcat(str, endTXStr);

	#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
	#endif

	#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
	#endif

	_sendArancinoCommand(str);
	char* message = _receiveArancinoResponse(END_TX_CHAR);
	#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
	#endif

	std::free(str);


	if (message == NULL) return communicationErrorPacket;

	ArancinoPacket packet = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
	std::free(message);

	return packet;
}

template<> char** ArancinoClass::mget(char** keys, uint len) {

	ArancinoPacket packet = mget<ArancinoPacket>(keys, len);

	if (packet.isError) return NULL;

	return packet.response.stringArray;
}

/******** API BASIC :: SET *********/

ArancinoPacket ArancinoClass::set( char* key, int value, bool isPersistent) {
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, uint32_t value, bool isPersistent) {
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, long value, bool isPersistent) {
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, float value, bool isPersistent) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, double value, bool isPersistent) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, char* value, bool isPersistent) {
	return __set(key, value, isPersistent);
}

ArancinoPacket ArancinoClass::__set( char* key, char* value, bool isPersistent) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && value != NULL && strcmp(key, "") != 0){

		int commandLength = 0;
		if(isPersistent){
			commandLength = strlen(SET_PERS_COMMAND);
		}
		else
		{
			commandLength = strlen(SET_COMMAND);
		}

		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int valueLength = strlen(value);
		int strLength = commandLength + 1 + keyLength + 1 + valueLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));
		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		if(isPersistent){
			strcpy(str, SET_PERS_COMMAND);
		} else
		{
			strcpy(str, SET_COMMAND);
		}

		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, dataSplitStr);
		strcat(str, value);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}

	}else{
			packet = invalidCommandErrorPacket;
	}

	return packet;
}

/******** API BASIC :: GET *********/

template<> ArancinoPacket ArancinoClass::get<ArancinoPacket>(char* key){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && strcmp(key, "") != 0 ){

		int commandLength = strlen(GET_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int strLength = commandLength + 1 + keyLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));
		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, GET_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif
		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> char* ArancinoClass::get(char* key){
	ArancinoPacket packet = get<ArancinoPacket>(key);

	char* retString;
	if (!packet.isError)
	{
		retString = packet.response.string;
	}
	else
	{
		retString = NULL;
	}
	return retString;
}

/******** API BASIC :: DEL *********/

template<> ArancinoPacket ArancinoClass::del<ArancinoPacket> (char* key){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && strcmp(key, "") != 0 ){
		int commandLength = strlen(DEL_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int strLength = commandLength + 1 + keyLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, DEL_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			char* messageParsed = _parse(message);
			ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
			packet = temp;
			std::free(messageParsed);
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> int ArancinoClass::del(char* key){
	ArancinoPacket packet = del<ArancinoPacket>(key);
	int retValue = 0;
	if (!packet.isError)
	{
		retValue = packet.response.integer;
	}
	return retValue;
}

/******** API BASIC :: HSET *********/

ArancinoPacket ArancinoClass::hset( char* key, char* field, int value ) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, float value ) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, double value ) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, uint32_t value ) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, long value ) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field , char* value) {

	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && field != NULL && value != NULL && strcmp(key, "") != 0 && strcmp(field, "") != 0){
		int commandLength = strlen(HSET_COMMAND);
		int keyLength;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int fieldLength = strlen(field);
		int valueLength = strlen(value);
		int strLength = commandLength + 1 + keyLength + 1 + fieldLength + 1 + valueLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));
		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif
		strcpy(str, HSET_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, dataSplitStr);
		strcat(str, field);
		strcat(str, dataSplitStr);
		strcat(str, value);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			packet.isError = 0;
			packet.responseCode = _getResponseCode(message);
			packet.responseType = VOID;
			packet.response.string = NULL;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

/******** API BASIC :: HGET *********/

template<> ArancinoPacket ArancinoClass::hget<ArancinoPacket> (char* key, char* field){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && field != NULL && strcmp(key, "") != 0 && strcmp(field, "") != 0 ){
		int commandLength = strlen(HGET_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int fieldLength = strlen(field);
		int strLength = commandLength + 1 + keyLength + 1 + fieldLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif
		strcpy(str, HGET_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, dataSplitStr);
		strcat(str, field);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}}; //TODO getStatus to _getResponseCode
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> char* ArancinoClass::hget(char* key, char* field){
	ArancinoPacket packet = hget<ArancinoPacket>(key, field);

	char* retString;
	if (!packet.isError)
	{
		retString = packet.response.string;
	}
	else
	{
		retString = NULL;
	}
	return retString;
}


/******** API BASIC :: HGETALL PACKET *********/

template<> ArancinoPacket ArancinoClass::hgetall<ArancinoPacket> (char* key){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && strcmp(key, "") != 0 ){
		int commandLength = strlen(HGETALL_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int strLength = commandLength + 1 + keyLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, HGETALL_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> char** ArancinoClass::hgetall(char* key){
	ArancinoPacket packet = hgetall<ArancinoPacket>(key);

	char** retArray;
	if (!packet.isError)
	{
		retArray = packet.response.stringArray;
	}
	else
	{
		retArray = NULL;
	}
	return retArray;
}

/******** API BASIC :: HKEYS *********/

template<> ArancinoPacket ArancinoClass::hkeys<ArancinoPacket> (char* key){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && strcmp(key, "") != 0 ){
		int commandLength = strlen(HKEYS_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int strLength = commandLength + 1 + keyLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, HKEYS_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet= invalidCommandErrorPacket;
	}

	return packet;
}

template<> char** ArancinoClass::hkeys(char* key){
	ArancinoPacket packet = hkeys<ArancinoPacket>(key);

	char** retArray;
	if (!packet.isError)
	{
		retArray = packet.response.stringArray;
	}
	else
	{
		retArray = NULL;
	}
	return retArray;
}

/******** API BASIC :: HVALS *********/

template<> ArancinoPacket ArancinoClass::hvals<ArancinoPacket> (char* key){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && strcmp(key, "") != 0 ){
		int commandLength = strlen(HVALS_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int strLength = commandLength + 1 + keyLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, HVALS_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet=invalidCommandErrorPacket;
	}

	return packet;
}

template<> char** ArancinoClass::hvals(char* key){
	ArancinoPacket packet = hvals<ArancinoPacket>(key);

	char** retArray;
	if (!packet.isError)
	{
		retArray = packet.response.stringArray;
	}
	else
	{
		retArray = NULL;
	}
	return retArray;
}

/******** API BASIC :: HDEL *********/

template<> ArancinoPacket ArancinoClass::hdel<ArancinoPacket> (char* key, char* field){
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(key != NULL && field != NULL && strcmp(key,"") && strcmp(field, "") != 0 ){
		int commandLength = strlen(HDEL_COMMAND);
		int keyLength ;
		if(arancino_id_prefix){
			keyLength = strlen(key)+idSize+1;
		}
		else{
			keyLength = strlen(key);
		}
		int fieldLength = strlen(field);
		int strLength = commandLength + 1 + keyLength + 1 + fieldLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, HDEL_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, key);
		strcat(str, dataSplitStr);
		strcat(str, field);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			char* messageParsed = _parse(message);
			ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
			packet = temp;
			std::free(messageParsed);
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> int ArancinoClass::hdel(char* key, char* field){
	ArancinoPacket packet = hdel<ArancinoPacket>(key, field);

	int retValue = 0;
	if (!packet.isError)
	{
		retValue = packet.response.integer;
	}
	return retValue;
}

/******** API BASIC :: KEYS PACKET *********/

template<> ArancinoPacket ArancinoClass::keys<ArancinoPacket> (char* pattern){
	ArancinoPacket packet;
	if(pattern != NULL && strcmp(pattern,"") != 0){
		int commandLength = strlen(KEYS_COMMAND);
		int patternLength = strlen(pattern);
		int strLength = commandLength + 1 + patternLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, KEYS_COMMAND);
		strcat(str, dataSplitStr);
		strcat(str, pattern);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
			packet = temp;
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

template<> char** ArancinoClass::keys(char* pattern){
	ArancinoPacket packet = keys<ArancinoPacket>(pattern);

	char** retArray;
	if (!packet.isError)
	{
		retArray = packet.response.stringArray;
	}
	else
	{
		retArray = NULL;
	}
	return retArray;
}

/******** API BASIC :: PUBLISH *********/

ArancinoPacket ArancinoClass::__publish(char* channel, char* msg) {
	if(_isReservedKey(channel)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	ArancinoPacket packet;
	if(channel != NULL && msg != NULL && strcmp(channel,"") != 0 ){
		int commandLength = strlen(PUBLISH_COMMAND);
		int channelLength ;
		if(arancino_id_prefix){
			channelLength = strlen(channel)+idSize+1;
		}
		else{
			channelLength = strlen(channel);
		}
		int msgLength = strlen(msg);
		int strLength = commandLength + 1 + channelLength + 1 + msgLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));

		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, PUBLISH_COMMAND);
		strcat(str, dataSplitStr);
		if(arancino_id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, channel);
		strcat(str, dataSplitStr);
		strcat(str, msg);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		if (message != NULL)
		{
			char* messageParsed = _parse(message);
			ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
			packet = temp;
			std::free(messageParsed);
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}
	}
	else{
		packet = invalidCommandErrorPacket;
	}

	return packet;
}

	ArancinoPacket ArancinoClass::publish(char* channel, char* msg){
		return __publish(channel, msg);
	}

	ArancinoPacket ArancinoClass::publish(char* channel, double msg){
		char str[20] = "";
		_doubleToString(msg, decimal_digits, str);

		return __publish(channel, str);
	}

	ArancinoPacket ArancinoClass::publish(char* channel, int msg){
		char str[20] = "";
		itoa(msg, str, 10);

		return __publish(channel, str);
	}

	ArancinoPacket ArancinoClass::publish(char* channel, uint32_t msg){
		char str[20] = "";
		itoa(msg, str, 10);

		return __publish(channel, str);
	}

	ArancinoPacket ArancinoClass::publish(char* channel, long msg){
		char str[20] = "";
		itoa(msg, str, 10);

		return __publish(channel, str);
	}

	/******** API BASIC :: FLUSH *********/

	ArancinoPacket ArancinoClass::flush() {
		int commandLength = strlen(FLUSH_COMMAND);
		int strLength = commandLength + 1 + 1;

		char* str = (char *)calloc(strLength, sizeof(char));
		#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			Serial.print(SENT_STRING);
		}
		#endif

		strcpy(str, FLUSH_COMMAND);
		strcat(str, endTXStr);

		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			vTaskSuspendAll();
		}
		#endif
		_sendArancinoCommand(str);
		char* message = _receiveArancinoResponse(END_TX_CHAR);
		#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
		if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
		{
			xTaskResumeAll();
		}
		#endif

		std::free(str);

		ArancinoPacket packet;

		if (message != NULL)
		{
			char* messageParsed = _parse(message);
			ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
			packet = temp;
			std::free(messageParsed);
			std::free(message);
		}
		else
		{
			packet = communicationErrorPacket;
		}

		return packet;

	}

	/******** API BASIC :: STORE *********/

	ArancinoPacket ArancinoClass::store( char* key, int value, char* tmstp) {
		char str[20] = "";
		itoa(value, str, 10);
		return __store(key, str, tmstp);
	}

	ArancinoPacket ArancinoClass::store( char* key, uint32_t value, char* tmstp) {
		char str[20] = "";
		itoa(value, str, 10);
		return __store(key, str, tmstp);
	}

	ArancinoPacket ArancinoClass::store( char* key, long value, char* tmstp) {
		char str[20] = "";
		itoa(value, str, 10);
		return __store(key, str, tmstp);
	}

	ArancinoPacket ArancinoClass::store( char* key, float value, char* tmstp) {
		char str[20] = "";
		_floatToString(value, decimal_digits, str);
		return __store(key, str, tmstp);
	}

	ArancinoPacket ArancinoClass::store( char* key, double value, char* tmstp) {
		char str[20] = "";
		return __store(key, str, tmstp);
	}

	ArancinoPacket ArancinoClass::store( char* key, int value) {
		char* ts = getTimestamp();
		return store(key, value, ts);
	}

	ArancinoPacket ArancinoClass::store( char* key, uint32_t value) {
		char* ts = getTimestamp();
		return store(key, value, ts);
	}

	ArancinoPacket ArancinoClass::store( char* key, long value) {
		char* ts = getTimestamp();
		return store(key, value, ts);
	}

	ArancinoPacket ArancinoClass::store( char* key, float value) {
		char* ts = getTimestamp();
		return store(key, value, ts);
	}

	ArancinoPacket ArancinoClass::store( char* key, double value) {
		char* ts = getTimestamp();
		return store(key, value, ts);
	}

	ArancinoPacket ArancinoClass::__store( char* key, char* value, char* tmstp) {
		if(_isReservedKey(key)){
			//TODO maybe it's better to print a log
			return reservedKeyErrorPacket;
		}
		ArancinoPacket packet;
		if(key != NULL && value != NULL && strcmp(key, "") != 0){

			int	commandLength = strlen(STORE_COMMAND);
			//
			int keyLength = strlen(key);
			int valueLength = strlen(value);
			int tmstpLength = strlen(tmstp);
			int strLength = commandLength + 1 + keyLength + 1 + valueLength + 1 + tmstpLength + 1 + 1;

			//set("ttss",tmstp);
			char* str = (char *)calloc(strLength, sizeof(char));
			#if defined(__SAMD21G18A__)
			if(!digitalRead(DBG_PIN)){
				Serial.print(SENT_STRING);
			}
			#endif

			strcpy(str, STORE_COMMAND);
			strcat(str, dataSplitStr);
			if(arancino_id_prefix){
				strcat(str, id);
				strcat(str, ID_SEPARATOR);
			}
			strcat(str, key);
			strcat(str, dataSplitStr);
			strcat(str, value);
			strcat(str, dataSplitStr);
			strcat(str, tmstp);
			strcat(str, endTXStr);

			#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
			if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
			{
				vTaskSuspendAll();
			}
			#endif
			_sendArancinoCommand(str);
			char* message = _receiveArancinoResponse(END_TX_CHAR);
			#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
			if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
			{
				xTaskResumeAll();
			}
			#endif

			std::free(str);

			if (message != NULL)
			{
				ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
				packet = temp;
				std::free(message);
			}
			else
			{
				packet = communicationErrorPacket;
			}

		}else{
				packet = invalidCommandErrorPacket;
		}

		return packet;
	}

/******** API UTILITY :: FREE *********/

void ArancinoClass::free(char* str){
	std::free(str);
}

void ArancinoClass::free(char** _array){
	char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;

	if (*_array != NULL)
		std::free(*_array);
	if (dummy != NULL)
		std::free(dummy);
}

void ArancinoClass::free(ArancinoPacket packet){
	if (packet.responseType == STRING)
	{
		std::free(packet.response.string);
	}
	else if (packet.responseType == STRING_ARRAY)
	{
		free(packet.response.stringArray);
	}
}
/******** API UTILITY :: CHECK-UTF8 *********/

bool ArancinoClass::isValidUTF8(const char * string) //From: https://stackoverflow.com/a/28311607
{
    if (!string)
        return true;

    const unsigned char * bytes = (const unsigned char *)string;
    unsigned int cp;
    int num;

    while (*bytes != 0x00)
    {
        if ((*bytes & 0x80) == 0x00)
        {
            // U+0000 to U+007F
            cp = (*bytes & 0x7F);
            num = 1;
        }
        else if ((*bytes & 0xE0) == 0xC0)
        {
            // U+0080 to U+07FF
            cp = (*bytes & 0x1F);
            num = 2;
        }
        else if ((*bytes & 0xF0) == 0xE0)
        {
            // U+0800 to U+FFFF
            cp = (*bytes & 0x0F);
            num = 3;
        }
        else if ((*bytes & 0xF8) == 0xF0)
        {
            // U+10000 to U+10FFFF
            cp = (*bytes & 0x07);
            num = 4;
        }
        else
            return false;

        bytes += 1;
        for (int i = 1; i < num; ++i)
        {
            if ((*bytes & 0xC0) != 0x80)
                return false;
            cp = (cp << 6) | (*bytes & 0x3F);
            bytes += 1;
        }

        if ((cp > 0x10FFFF) ||
            ((cp >= 0xD800) && (cp <= 0xDFFF)) ||
            ((cp <= 0x007F) && (num != 1)) ||
            ((cp >= 0x0080) && (cp <= 0x07FF) && (num != 2)) ||
            ((cp >= 0x0800) && (cp <= 0xFFFF) && (num != 3)) ||
            ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (num != 4)))
            return false;
    }

    return true;
}
/******** API ADVANCED :: PRINT *********/

void ArancinoClass::print(char* value){
	_sendViaCOMM_MODE(MONITOR_KEY, value);
}

void ArancinoClass::print(String value){
	_sendViaCOMM_MODE(MONITOR_KEY, value.begin());
}

void ArancinoClass::print(int value) {
	char str[20] = "";
	itoa(value, str, 10);
	print(str);
}

void ArancinoClass::print(float value) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	print(str);
}

void ArancinoClass::print(double value) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	print(str);
}

/******** API ADVANCED :: PRINTLN *********/

void ArancinoClass::println(String value) {
	print(value+String('\n'));
}

void ArancinoClass::println(int value) {
	print(String(value)+String('\n'));
}

void ArancinoClass::println(float value) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	print(str+String('\n'));
}

void ArancinoClass::println(double value) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	print(str+String('\n'));
}

// ????

int ArancinoClass::getArraySize(char** _array) {
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  return dummy != NULL ? (int)(*dummy) : 0;
}

int ArancinoClass::getArraySize(String* _array) {
	String* dummy = (_array != NULL) ? _array - 1 : NULL;
	return (dummy[0] != "") ? dummy[0].toInt() : 0;
}

char* ArancinoClass::getTimestamp() {

	//unsigned long ts = rtc.getEpoch(); //10 digits
	unsigned long millis_current = millis();
	unsigned long mls_tmp = millis_current - millis_previous;
	millis_previous=millis_current;
	if(mls_tmp < 0){	//overflow of millis
		mls_tmp=MAX_UNSIGNED_LONG_VALUE-millis_previous+millis_current;
	}
	tmst_inf=tmst_inf+mls_tmp;
	if(tmst_inf > 999999999){
		tmst_sup=tmst_sup+1;
		tmst_inf=tmst_inf % 1000000000;
	}
	char ts_sup_tmp[4];
	sprintf(ts_sup_tmp, "%04d", tmst_sup); //4 digits 
	memcpy(timestamp,ts_sup_tmp,4);
	char ts_inf_tmp[9];
	sprintf(ts_inf_tmp, "%09d", tmst_inf); //9 digits
	memcpy(&timestamp[4],ts_inf_tmp,9);
	return timestamp;
}

/*char* ArancinoClass::getTimestamp() {

	unsigned long ts = rtc.getEpoch(); //10 digits
	unsigned long mls_tmp = (millis() - millis_start) % 1000;
	unsigned int ts_inf = ts % 1000000000; //last 9 digits of epoch
  unsigned int ts_sup = ts/1000000000; //first digits of epoch
	unsigned int mls =mls_tmp; //+ timestampMillis ; //millis + delta;
	if(mls > 999){ //millis + delta is greater than 1 seconds (1000)
		ts_inf=ts_inf+1;
		if(ts_inf> 999999999){
			ts_sup=ts_sup+1;
			ts_inf=0;
		}
	}
	mls=mls % 1000; //last 3 digits of millis

	itoa(ts_sup, timestamp, 10);
	char ts_tmp[9];
	sprintf(ts_tmp, "%09d", ts_inf);
	strcat(timestamp, ts_tmp);
	char mill[6];
	sprintf(mill, "%03d", mls);
	strcat(timestamp, mill);
	//itoa(ts, timestamp, 10);
	return timestamp;
}*/

/******** INTERNAL UTILS :: FREE *********/

void ArancinoClass::_sendArancinoCommand(char* command) {
	//check communication timeout with arancino module
	if (comm_timeout){
		/*
			Flush data on serial communication to avoid of lost
			synchronization between arancino library and arancino module.
			By this way I prevent to receive reposonse of a previous sent command.
		*/
		while(SERIAL_PORT.available() > 0){
				SERIAL_PORT.read();
		}
		comm_timeout=false;
	}
	//command must terminate with '\0'!
	SERIAL_PORT.write(command, strlen(command)); //excluded '\0'
	#if defined(__SAMD21G18A__)
		if(!digitalRead(DBG_PIN)){
			if(command[strlen(command) - 1] == END_TX_CHAR)
			{
				Serial.println(command);
			}
			else
				Serial.print(command);
		}
	#endif
}

void ArancinoClass::_sendArancinoCommand(char command) {
	char* c = (char *)calloc(2, sizeof(char));
	c[0] = command;
	c[1] = '\0';
	_sendArancinoCommand(c);
	std::free(c);
}

/*
 * 'terminator' char is used only for non-freeRTOS implementations.
 * For freeRTOS implementations is always used END_TX_CHAR as terminator char (see commTask()).
 */
char* ArancinoClass::_receiveArancinoResponse(char terminator) {
	char* response = NULL; //must be freed
	String str = "";
	str = SERIAL_PORT.readStringUntil(terminator);
	if( str == ""){
		//enable timeout check
		comm_timeout = true;
	}
	else {
		int responseLength = strlen(str.begin());
		if (responseLength > 0)
		{
			response = (char *)calloc(responseLength + 1 + 1, sizeof(char));
			strcpy(response, str.begin());
			response[responseLength] = END_TX_CHAR;
			response[responseLength + 1] = '\0';
		}
	}
	return response;
}

bool ArancinoClass::_isReservedKey(char* key) {
	//int keyCount = sizeof(reservedKey) / sizeof(reservedKey[0]);
	for(int i = 0; i < RESERVED_KEY_ARRAY_SIZE; i++)
	{
		if(strcmp(reservedKey[i], key) == 0)
			return true;
	}
	return false;
}

void ArancinoClass::_doubleToString(double value, unsigned int _nDecimal, char* str) {
	char val[20]="";
	sprintf(val, "%d", int(value));
	int valueLength=strlen(val);
	if(valueLength+_nDecimal>15){			//The double data type has 15 decimal digits of precision
      _nDecimal=15-valueLength;
      if(_nDecimal<0){
        _nDecimal=0;
      }
  }
	dtostrf(value, valueLength, _nDecimal, str);
}

void ArancinoClass::_floatToString(float value, unsigned int _nDecimal, char* str) {
	char val[20]="";
	sprintf(val, "%d", int(value));
	int valueLength=strlen(val);
	if(valueLength+_nDecimal>7){			//The float data type has 7 decimal digits of precision
      _nDecimal=7-valueLength;
      if(_nDecimal<0){
        _nDecimal=0;
      }
  }
	dtostrf(value, valueLength, _nDecimal, str);
}

int ArancinoClass::_getDigit(long value) {
	int digit = 0;
	long _val = value;
	while(abs(_val) > 0)
	{
		++digit;
		_val /= 10;
	}
	return digit;
}

ArancinoPacket ArancinoClass::_sendViaCOMM_MODE(char* key, char* value, bool isPersistent) {
	switch (COMM_MODE) {
		case SYNCH:
			return __set(key, value, isPersistent);
		break;

		case ASYNCH:
			return __publish(key, value);
		break;

		case BOTH:
			return __publish(key, value);
			return __set(key, value, isPersistent);
		break;

		default:
			return __set(key, value, isPersistent);
		break;
	}
}

int ArancinoClass::_getResponseCode(char* message) {
	int value = -1;
	int separatorIndex = -1;
	char* temp = NULL;
	char* charAddr = strchr(message, DATA_SPLIT_CHAR); //address of DATA_SPLIT_CHAR
	if (charAddr == NULL) //NO DATA_SPLIT_CHAR
	{
		charAddr = strchr(message, END_TX_CHAR); //-> search for END_TX_CHAR
	}

	if (charAddr != NULL)
	{
	  separatorIndex = charAddr - message; //index of DATA_SPLIT_CHAR/END_TX_CHAR on message string
	  temp = (char *)calloc(separatorIndex + 1, sizeof(char));
	  strncpy(temp, message, separatorIndex + 1);
	  temp[separatorIndex] = '\0'; //replace separator with null-character

	  value = atoi(temp);
	  std::free(temp);
	}
	else
	{
		value = -1; //error
	}

	return value;
}

char* ArancinoClass::_parse(char* message) {

	char* status = NULL;
	char* value = NULL;
	int DSCIndex; //DATA_SPLIT_CHAR index
	char* charAddr = strchr(message, DATA_SPLIT_CHAR); //address of DATA_SPLIT_CHAR

	if (charAddr != NULL)
	{
	  DSCIndex = charAddr - message; //index of DATA_SPLIT_CHAR on message string
	}
	else
	{
	  DSCIndex = -1; //DATA_SPLIT_CHAR not found --> received only 'status'
	}

	int messageLength = strlen(message);

	if (DSCIndex == -1)
	{
		status = (char *)calloc(messageLength + 1, sizeof(char)); //response = [STATUS] + @
		strncpy(status, message, messageLength);
		status[messageLength] = '\0'; //replace END_TX_CHAR with '\0'
	}
	else
	{
		status = (char *)calloc(DSCIndex + 1, sizeof(char)); //response = [STATUS] + # + [VALUE] + @
		strncpy(status, message, DSCIndex);
		status[DSCIndex] = '\0'; //replace DATA_SPLIT_CHAR with '\0'

		value = (char *)calloc(messageLength - DSCIndex - 1, sizeof(char));
		strncpy(value, &message[DSCIndex + 1], messageLength - (DSCIndex + 2));
		value[messageLength - (DSCIndex + 2)] = '\0'; //replace END_TX_CHAR with '\0'
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

	std::free(status);

	if (strcmp(value, nullStr) == 0){
		std::free(value);
		return NULL;
	}else{
		return value;
	}

}

char** ArancinoClass::_parseArray(char* data) {
	char** arrayParsed = NULL;
	char* tempArray;

	char* previousDSCIndex = data;
	char* DSCIndex = data;

	int fieldCount = data != NULL; //at least 1 field
	int maxLength = 0;

	if (data != NULL) {

		//get the key count
		do
		{
			DSCIndex = strchr(previousDSCIndex, DATA_SPLIT_CHAR);

			if (DSCIndex != NULL)
			{
				++fieldCount;
				if (DSCIndex - previousDSCIndex > maxLength)
					maxLength = DSCIndex - previousDSCIndex;
			}
			else if (strlen(previousDSCIndex) > maxLength)
			{
				maxLength =  strlen(previousDSCIndex);
			}
			previousDSCIndex = DSCIndex + 1;


		} while(DSCIndex != NULL);

		/*
		 * Alloco un elemento in più (fieldCount + 1).
		 * Nel primo elemento inserisco la lunghezza del vettore;
		 * Il valore di ritorno della funzione sarà l'elemento 1 del vettore (e non l'elemento 0)
		 */
		arrayParsed = (char **)malloc((fieldCount + 1) * sizeof(char*)); //user must free!
		tempArray = (char *)malloc((fieldCount + 1) * (maxLength + 1) * sizeof(char)); //user must free!
		arrayParsed[0] = (char*)fieldCount;


		previousDSCIndex = data;

		for (int i = 1; i < (fieldCount + 1); ++i)
		{
			arrayParsed[i] = tempArray + ((i - 1) * (maxLength + 1));
			DSCIndex = strchr(previousDSCIndex + 1, DATA_SPLIT_CHAR);

			if (DSCIndex != NULL)
			{
				if (strncmp(previousDSCIndex, nullStr, DSCIndex - previousDSCIndex) == 0){
					arrayParsed[i] = NULL;
				} else {
					strncpy(arrayParsed[i], previousDSCIndex, DSCIndex - previousDSCIndex);
					arrayParsed[i][DSCIndex - previousDSCIndex] = '\0';
				}
				previousDSCIndex = DSCIndex + 1;
			}
			else
			{
				strcpy(arrayParsed[i], previousDSCIndex);
			}
		}

		if (data != NULL) {
			std::free(data);
		}
	}

	return (data != NULL && arrayParsed != NULL) ? &arrayParsed[1] : NULL;
}

/*void ArancinoClass::dropAll() {
  while (stream.available() > 0) {
	stream.read();
  }
}*/

ArancinoClass Arancino;
