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
#define DEBUG 0

ArancinoPacket reservedKeyErrorPacket = {true, RESERVED_KEY_ERROR, RESERVED_KEY_ERROR, {.string = NULL}}; //default reserved key error packet
ArancinoPacket communicationErrorPacket = {true, COMMUNICATION_ERROR, COMMUNICATION_ERROR, {.string = NULL}}; //default reserved key error packet

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

/******** API BASIC :: BEGIN *********/

void ArancinoClass::begin(int timeout) {
	SERIAL_PORT.begin(BAUDRATE);
	SERIAL_PORT.setTimeout(TIMEOUT);

	int commandLength = strlen(START_COMMAND);
	int argLength = strlen(LIB_VERSION);
	int strLength = commandLength + 1 + argLength + 1 + 1;
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
	
	strcpy(str, START_COMMAND);
	strcat(str, dataSplitStr);
	strcat(str, LIB_VERSION);
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
			ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
			packet = temp;
		}
		else
		{
			packet = communicationErrorPacket;
		}
		std::free(message);
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

/******** API BASIC :: SET *********/

ArancinoPacket ArancinoClass::set( char* key, int value) {
	return set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, double value) {
	return set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, uint32_t value) {
	return set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, char* value) {
	return __set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, int value, bool isPersistent ) {
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, double value, bool isPersistent ) {
	char str[20] = "";
	_doubleToString(value, 4, str);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, uint32_t value, bool isPersistent) {
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, char* value, bool isPersistent ) {
	return __set(key, value, isPersistent);
}

ArancinoPacket ArancinoClass::__set( char* key, char* value, bool isPersistent ) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	int commandLength = 0;

	if(isPersistent){
		commandLength = strlen(SET_PERS_COMMAND);
	}
	else
	{
		commandLength = strlen(SET_COMMAND);
	}
	
	int keyLength = strlen(key);
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
	
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
	if (strcmp(value, "") != 0)
	{
		strcat(str, dataSplitStr);
		strcat(str, value);

	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
		packet = temp;
		std::free(message);
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

/******** API BASIC :: GET *********/

ArancinoPacket ArancinoClass::_getPacket( char* key ) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(GET_COMMAND);
	int keyLength = strlen(key);
	int strLength = commandLength + 1 + keyLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, GET_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}};
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char* ArancinoClass::_get(char* key) {
	ArancinoPacket packet = _getPacket(key);
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

template<> ArancinoPacket ArancinoClass::get<ArancinoPacket>(char* key){
	return _getPacket(key);
}

template<> char* ArancinoClass::get(char* key){
	return _get(key);
}

/******** API BASIC :: DEL *********/

ArancinoPacket ArancinoClass::_delPacket(char* key) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(DEL_COMMAND);
	int keyLength = strlen(key);
	int strLength = commandLength + 1 + keyLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, DEL_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
		packet = temp;
		std::free(messageParsed);
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

int ArancinoClass::_del( char* key ) {
	ArancinoPacket packet = _delPacket(key);
	int retValue = 0;
	if (!packet.isError)
	{
		retValue = packet.response.integer;
	}
	return retValue;
}

template<> ArancinoPacket ArancinoClass::del<ArancinoPacket> (char* key){
	return _delPacket(key);
}

template<> int ArancinoClass::del(char* key){
	return _del(key);
}

/******** API BASIC :: HSET *********/

ArancinoPacket ArancinoClass::hset( char* key, char* field, int value ) {
	char str[20]; 
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, double value ) {
	char str[20] = "";
	_doubleToString(value, 4, str);
	return hset(key, field, str);	
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, uint32_t value ) {
	char str[20]; 
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field , char* value) {

	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	int commandLength = strlen(HSET_COMMAND);
	int keyLength = strlen(key);
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
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
	if (strcmp(field, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, field);
	}
	if (strcmp(value, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, value);
	}
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

	return packet;
}

/******** API BASIC :: HGET *********/

ArancinoPacket ArancinoClass::_hgetPacket( char* key, char* field ) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	int commandLength = strlen(HGET_COMMAND);
	int keyLength = strlen(key);
	int fieldLength = strlen(field);
	int strLength = commandLength + 1 + keyLength + 1 + fieldLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	strcpy(str, HGET_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
	if (strcmp(field, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, field);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}}; //TODO getStatus to _getResponseCode
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char* ArancinoClass::_hget( char* key, char* field ) {
	ArancinoPacket packet = _hgetPacket(key, field);
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

template<> ArancinoPacket ArancinoClass::hget<ArancinoPacket> (char* key, char* field){
	return _hgetPacket(key, field);
}

template<> char* ArancinoClass::hget(char* key, char* field){
	return _hget(key, field);
}


/******** API BASIC :: HGETALL PACKET *********/

ArancinoPacket ArancinoClass::_hgetallPacket(char* key) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(HGETALL_COMMAND);
	int keyLength = strlen(key);
	int strLength = commandLength + 1 + keyLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, HGETALL_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char** ArancinoClass::_hgetall(char* key) {
	ArancinoPacket packet = _hgetallPacket(key);
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

template<> ArancinoPacket ArancinoClass::hgetall<ArancinoPacket> (char* key){
	return _hgetallPacket(key);
}

template<> char** ArancinoClass::hgetall(char* key){
	return _hgetall(key);
}

/******** API BASIC :: HKEYS *********/

ArancinoPacket ArancinoClass::_hkeysPacket(char* key) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(HKEYS_COMMAND);
	int keyLength = strlen(key);
	int strLength = commandLength + 1 + keyLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, HKEYS_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char** ArancinoClass::_hkeys(char* key) {
	ArancinoPacket packet = _hkeysPacket(key);
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

template<> ArancinoPacket ArancinoClass::hkeys<ArancinoPacket> (char* key){
	return _hkeysPacket(key);
}

template<> char** ArancinoClass::hkeys(char* key){
	return _hkeys(key);
}

/******** API BASIC :: HVALS *********/

ArancinoPacket ArancinoClass::_hvalsPacket(char* key) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(HVALS_COMMAND);
	int keyLength = strlen(key);
	int strLength = commandLength + 1 + keyLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, HVALS_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char** ArancinoClass::_hvals(char* key) {
	ArancinoPacket packet = _hvalsPacket(key);
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

template<> ArancinoPacket ArancinoClass::hvals<ArancinoPacket> (char* key){
	return _hvalsPacket(key);
}

template<> char** ArancinoClass::hvals(char* key){
	return _hvals(key);
}

/******** API BASIC :: HDEL *********/

ArancinoPacket ArancinoClass::_hdelPacket( char* key, char* field) {
	if(_isReservedKey(key)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}

	int commandLength = strlen(HDEL_COMMAND);
	int keyLength = strlen(key);
	int fieldLength = strlen(field);
	int strLength = commandLength + 1 + keyLength + 1 + fieldLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, HDEL_COMMAND);
	if (strcmp(key, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, key);
	}
	if (strcmp(field, "") != 0)
	{
		strcat(str, dataSplitStr);
		strcat(str, field);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
		packet = temp;
		std::free(messageParsed);
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet; 
}

int ArancinoClass::_hdel(char* key, char* field) {
	ArancinoPacket packet = _hdelPacket(key, field);
	int retValue = 0;
	if (!packet.isError)
	{
		retValue = packet.response.integer;
	}
	return retValue;
}

template<> ArancinoPacket ArancinoClass::hdel<ArancinoPacket> (char* key, char* field){
	return _hdelPacket(key, field);
}

template<> int ArancinoClass::hdel(char* key, char* field){
	return _hdel(key, field);
}

/******** API BASIC :: KEYS PACKET *********/

ArancinoPacket ArancinoClass::_keysPacket(char* pattern){
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
	if (strcmp(pattern, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, pattern);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
		packet = temp;
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

char** ArancinoClass::_keys(char* pattern){
	ArancinoPacket packet = _keysPacket(pattern);
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

template<> ArancinoPacket ArancinoClass::keys<ArancinoPacket> (char* pattern){
	return _keysPacket(pattern);
}

template<> char** ArancinoClass::keys(char* pattern){
	return _keys(pattern);
}

/******** API BASIC :: PUBLISH *********/

ArancinoPacket ArancinoClass::_publishPacket(int channel, char* msg) {
	char str[20] = "";
	itoa(channel, str, 10);
	return __publish(str, msg);
}

ArancinoPacket ArancinoClass::_publishPacket(char* channel, char* msg) {
	return __publish(channel, msg);
}

// int ArancinoClass::_publish(int channel, char* msg) {
// 	ArancinoPacket packet = _publishPacket(channel, msg);
// 	int retValue = 0;
// 	if (!packet.isError)
// 	{
// 		retValue = packet.response.integer;
// 	}
// 	return retValue;
// }

// int ArancinoClass::_publish(char* channel, char* msg) {
// 	ArancinoPacket packet = _publishPacket(channel, msg);
// 	int retValue = 0;
// 	if (!packet.isError)
// 	{
// 		retValue = packet.response.integer;
// 	}
// 	return retValue;
// }

ArancinoPacket ArancinoClass::__publish(char* channel, char* msg) {
	if(_isReservedKey(channel)){
		//TODO maybe it's better to print a log
		return reservedKeyErrorPacket;
	}
	
	int commandLength = strlen(PUBLISH_COMMAND);
	int channelLength = strlen(channel);
	int msgLength = strlen(msg);
	int strLength = commandLength + 1 + channelLength + 1 + msgLength + 1 + 1;
	
	char* str = (char *)calloc(strLength, sizeof(char));  
	
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	
	strcpy(str, PUBLISH_COMMAND);
	if (strcmp(channel, "") != 0){
		strcat(str, dataSplitStr);
		strcat(str, channel);
	}
	if (strcmp(msg, "") != 0)
	{
		strcat(str, dataSplitStr);
		strcat(str, msg);
	}
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
		ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
		packet = temp;
		std::free(messageParsed);
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;
}

ArancinoPacket ArancinoClass::publish(int channel, char* msg){
	return _publishPacket(channel, msg);
}

ArancinoPacket ArancinoClass::publish(char* channel, char* msg){
	return _publishPacket(channel, msg);
}

// template<> int ArancinoClass::publish(int channel, char* msg){
// 	return _publish(channel, msg);
// }

// template<> int ArancinoClass::publish(char* channel, char* msg){
// 	return _publish(channel, msg);
// }


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
	}
	else
	{
		packet = communicationErrorPacket;
	}

	return packet;

}

/******** API UTILITY :: FREE *********/

void ArancinoClass::free(char* str){
	std::free(str);
}

void ArancinoClass::free(char** _array){
	_freeArray(_array);
}

void ArancinoClass::free(ArancinoPacket packet){
	_freePacket(packet);
}

void ArancinoClass::_freePacket(ArancinoPacket packet){
	if (packet.responseType == STRING)
	{
		std::free(packet.response.string);
	}
	else if (packet.responseType == STRING_ARRAY)
	{
		_freeArray(packet.response.stringArray);
	}
	else
	{
		//nothing to do
	}
}

void ArancinoClass::_freeArray(char** _array) {
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  if (*_array != NULL)
	std::free(*_array);
  if (dummy != NULL)
	std::free(dummy);
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

/******** API ADVANCED :: PRINTLN *********/

void ArancinoClass::print(double value) {
	char str[20] = "";
	_doubleToString(value, 4, str);
	print(str);
}

void ArancinoClass::println(String value) {
	print(value+String('\n'));
}

void ArancinoClass::println(int value) {
	print(String(value));
	print("\n");
}

void ArancinoClass::println(double value) {
	char str[20] = "";
	_doubleToString(value, 4, str);
	print(str);
	print("\n");
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



/******** INTERNAL UTILS :: FREE *********/

void ArancinoClass::_sendArancinoCommand(char* command) {
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
	String str = SERIAL_PORT.readStringUntil(terminator);
	int responseLength = strlen(str.begin());
	if (responseLength > 0)
	{
		response = (char *)calloc(responseLength + 1 + 1, sizeof(char));
		strcpy(response, str.begin());
		response[responseLength] = END_TX_CHAR;
		response[responseLength + 1] = '\0';
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

void ArancinoClass::_doubleToString(double value, unsigned int _nDecimal, char* str) {//truncation!
	uint8_t sign = (value < 0);
	uint8_t dot = (_nDecimal > 0);
	long integer = ( sign ? ceil(value) : floor(value) );
	double _decimal = (value - integer); //without integer part
	integer = abs(integer);
	
	for (int i = 0; i < _nDecimal; i++)
		_decimal *= 10;

	long decimal = (_decimal < 0) ? ceil(_decimal - 0.5) : floor(_decimal + 0.5);
	decimal = abs(decimal);
	
	int integerDigit = integer != 0 ? _getDigit(integer) : 1;
	int decimalDigit = _nDecimal;
	
	str[0] = '-';
	ltoa(integer, &str[sign], 10);
	if (dot)
	{
		str[sign + integerDigit] = '.';
		ltoa(decimal, &str[sign + integerDigit + 1], 10);
		for (int i = 1; decimal == 0 && i < decimalDigit; i++)
			ltoa(decimal, &str[sign + integerDigit + i + 1], 10);
	}
	str[sign + integerDigit + dot + decimalDigit] = '\0';
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

ArancinoPacket ArancinoClass::_sendViaCOMM_MODE(char* key, char* value) {
	return _sendViaCOMM_MODE(key, value, false);
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

	std::free(message);
	std::free(status);
	return value;

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
				strncpy(arrayParsed[i], previousDSCIndex, DSCIndex - previousDSCIndex);
				arrayParsed[i][DSCIndex - previousDSCIndex] = '\0';  
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
