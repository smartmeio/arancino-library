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


/*ArancinoClass::ArancinoClass(Stream &_stream):
	stream(_stream), started(false) {
  // Empty
}*/

/*void ArancinoClass::begin() {
	begin(TIMEOUT);
}*/

//============= SETUP FUNCTIONS ======================

void ArancinoClass::begin(int timeout) {
    SERIAL_PORT.begin(BAUDRATE);
    SERIAL_PORT.setTimeout(TIMEOUT);

    int commandLength = strlen(START_COMMAND);
    int strLength = commandLength + 1 + 1;
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
	strcat(str, endTXStr);
    
    ArancinoPacket packet;
    // Start communication with serial module on CPU
    do{
        #if defined(__SAMD21G18A__)
        if(!digitalRead(DBG_PIN)){
            Serial.print(SENT_STRING);
        }
        #endif
        
        
        sendArancinoCommand(str);
        char* message = receiveArancinoResponse(END_TX_CHAR);
        
        if (message != NULL)
        {
            ArancinoPacket temp = {false, getResponseCode(message), VOID, {.string = NULL}};
            packet = temp;
        }
        else
        {
            packet = communicationErrorPacket;
        }
        free(message);
    }while (packet.isError == true || packet.responseCode != RSP_OK);
    free(str);

    sendViaCOMM_MODE(LIBVERS_KEY, LIB_VERSION);

}


#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
TaskHandle_t commTaskHandle;

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


void ArancinoClass::setReservedCommunicationMode(int mode){
	COMM_MODE = mode;
}

//============= API FUNCTIONS ======================

/*String ArancinoClass::get( String key ) {

}*/


// ==== GET

ArancinoPacket ArancinoClass::getPacket( char* key ) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING, {.string = parse(message)}};
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char* ArancinoClass::get( char* key ) {
    ArancinoPacket packet = getPacket(key);
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

// ==== SET

ArancinoPacket ArancinoClass::_set( char* key, char* value, bool isPersistent ) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), VOID, {.string = NULL}};
        packet = temp;
        free(message);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

ArancinoPacket ArancinoClass::set( char* key, char* value) {
    return _set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, int value) {
    return set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, double value) {
    return set(key, value, false);
}

ArancinoPacket ArancinoClass::set( char* key, uint32_t value) {
    return set(key, value, false);
}


ArancinoPacket ArancinoClass::set( char* key, char* value, bool isPersistent ) {
	return _set(key, value, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, int value, bool isPersistent ) {
    char str[20] = "";
    itoa(value, str, 10);
	return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, double value, bool isPersistent ) {
    char str[20] = "";
    doubleToString(value, 4, str);
    return set(key, str, isPersistent);
}

ArancinoPacket ArancinoClass::set( char* key, uint32_t value, bool isPersistent) {
    char str[20] = "";
    itoa(value, str, 10);
	return set(key, str, isPersistent);
}


// ==== DEL

ArancinoPacket ArancinoClass::delPacket( char* key ) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);
    
    ArancinoPacket packet;
    
    if (message != NULL)
    {
        char* messageParsed = parse(message);
        ArancinoPacket temp = {false, getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
        packet = temp;
        free(messageParsed);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

int ArancinoClass::del( char* key ) {
    ArancinoPacket packet = delPacket(key);
    int retValue = 0;
    if (!packet.isError)
    {
        retValue = packet.response.integer;
    }
    return retValue;
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
	String message = receiveArancinoResponse(END_TX_CHAR);

	return parse(message).toInt();
}*/


// ==== HGET

ArancinoPacket ArancinoClass::hgetPacket( char* key, char* field ) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING, {.string = parse(message)}}; //TODO getStatus to getResponseCode
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char* ArancinoClass::hget( char* key, char* field ) {
    ArancinoPacket packet = hgetPacket(key, field);
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

// ==== HGETALL

ArancinoPacket ArancinoClass::hgetallPacket(char* key) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);
    
    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING_ARRAY, {.stringArray = parseArray(parse(message))}};
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char** ArancinoClass::hgetall(char* key) 
{
    ArancinoPacket packet = hgetallPacket(key);
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

// ==== HKEYS

ArancinoPacket ArancinoClass::hkeysPacket(char* key) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);
    
    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING_ARRAY, {.stringArray = parseArray(parse(message))}};
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char** ArancinoClass::hkeys(char* key) {
    ArancinoPacket packet = hkeysPacket(key);
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

// ==== HVALS

ArancinoPacket ArancinoClass::hvalsPacket(char* key) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif
    
    free(str);
    
    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING_ARRAY, {.stringArray = parseArray(parse(message))}};
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char** ArancinoClass::hvals(char* key) {
    ArancinoPacket packet = hvalsPacket(key);
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


// ==== KEYS

ArancinoPacket ArancinoClass::keysPacket(char* pattern){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        ArancinoPacket temp = {false, getResponseCode(message), STRING_ARRAY, {.stringArray = parseArray(parse(message))}};
        packet = temp;
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

char** ArancinoClass::keys(char* pattern){
    ArancinoPacket packet = keysPacket(pattern);
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


ArancinoPacket ArancinoClass::hset( char* key, char* field , char* value) {

    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        packet.isError = 0; 
        packet.responseCode = getResponseCode(message);
        packet.responseType = VOID;
        packet.response.string = NULL;
        free(message);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

// ==== HSET

ArancinoPacket ArancinoClass::hset( char* key, char* field, int value ) {
    char str[20]; 
    itoa(value, str, 10);
    return hset(key, field, str);
}


ArancinoPacket ArancinoClass::hset( char* key, char* field, double value ) {
    char str[20] = "";
    doubleToString(value, 4, str);
    return hset(key, field, str);    
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, uint32_t value ) {
    char str[20]; 
    itoa(value, str, 10);
    return hset(key, field, str);
}

// ==== HDEL

ArancinoPacket ArancinoClass::hdelPacket( char* key, char* field ) {
    if(isReservedKey(key)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        char* messageParsed = parse(message);
        ArancinoPacket temp = {false, getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
        packet = temp;
        free(messageParsed);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet; 
}


int ArancinoClass::hdel(char* key, char* field) {
    ArancinoPacket packet = hdelPacket(key, field);
    int retValue = 0;
    if (!packet.isError)
    {
        retValue = packet.response.integer;
    }
    return retValue;
}

// ==== PUB

ArancinoPacket ArancinoClass::_publish( char* channel, char* msg ) {
    if(isReservedKey(channel)){
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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        char* messageParsed = parse(message);
        ArancinoPacket temp = {false, getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
        packet = temp;
        free(messageParsed);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;
}

ArancinoPacket ArancinoClass::publishPacket( char* channel, char* msg ) {
	return _publish(channel, msg);
}

int ArancinoClass::publish( char* channel, char* msg ) {
    ArancinoPacket packet = publishPacket(channel, msg);
    int retValue = 0;
    if (!packet.isError)
    {
        retValue = packet.response.integer;
    }
    return retValue;
}

ArancinoPacket ArancinoClass::publishPacket( int channel, char* msg ) {
    char str[20] = "";
    itoa(channel, str, 10);
	return _publish(str, msg);
}

int ArancinoClass::publish( int channel, char* msg ) {
    ArancinoPacket packet = publishPacket(channel, msg);
    int retValue = 0;
    if (!packet.isError)
    {
        retValue = packet.response.integer;
    }
    return retValue;
}

// ==== FLUSH

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
    sendArancinoCommand(str);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
        xTaskResumeAll();
    }
    #endif  
    
    free(str);

    ArancinoPacket packet;
    
    if (message != NULL)
    {
        char* messageParsed = parse(message);
        ArancinoPacket temp = {false, getResponseCode(message), VOID, {.string = NULL}};
        packet = temp;
        free(messageParsed);
    }
    else
    {
        packet = communicationErrorPacket;
    }

	return packet;

}

int ArancinoClass::getArraySize(char** _array) {
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  return dummy != NULL ? (int)(*dummy) : 0;
}

int ArancinoClass::getArraySize(String* _array) {
    String* dummy = (_array != NULL) ? _array - 1 : NULL;
    return (dummy[0] != "") ? dummy[0].toInt() : 0;
}

/*void ArancinoClass::free(char* str)
{
    free(str);
}*/

/*void ArancinoClass::free(char** _array)
{
    freeArray(_array);
}*/

void ArancinoClass::freePacket(ArancinoPacket packet)
{
    if (packet.responseType == STRING)
    {
        free(packet.response.string);
    }
    else if (packet.responseType == STRING_ARRAY)
    {
        freeArray(packet.response.stringArray);
    }
    else
    {
        //nothing to do
    }
}

void ArancinoClass::freeArray(String* _array){
    String* dummy = (_array != NULL) ? _array - 1 : NULL;
    delete[] dummy;
}

void ArancinoClass::freeArray(char** _array) {
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  if (*_array != NULL)
    free(*_array);
  if (dummy != NULL)
    free(dummy);
}


int ArancinoClass::getDigit(long value) {
    int digit = 0;
    long _val = value;
    while(abs(_val) > 0)
    {
        ++digit;
        _val /= 10;
    }
    return digit;
}

void ArancinoClass::doubleToString(double value, unsigned int _nDecimal, char* str) {//truncation!
    uint8_t sign = (value < 0);
    uint8_t dot = (_nDecimal > 0);
    long integer = ( sign ? ceil(value) : floor(value) );
    double _decimal = (value - integer); //without integer part
    integer = abs(integer);
    
    for (int i = 0; i < _nDecimal; i++)
        _decimal *= 10;

    long decimal = (_decimal < 0) ? ceil(_decimal - 0.5) : floor(_decimal + 0.5);
    decimal = abs(decimal);
    
    int integerDigit = integer != 0 ? getDigit(integer) : 1;
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


//============= DEBUG FUNCTIONS ======================

void ArancinoClass::print(char* value){
	sendViaCOMM_MODE(MONITOR_KEY, value);
}

void ArancinoClass::print(String value){
	sendViaCOMM_MODE(MONITOR_KEY, value.begin());
}

void ArancinoClass::print(int value) {
    char str[20] = "";
    itoa(value, str, 10);
	print(str);
}

void ArancinoClass::print(double value) {
    char str[20] = "";
    doubleToString(value, 4, str);
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
    doubleToString(value, 4, str);
	print(str);
    print("\n");
}

ArancinoPacket ArancinoClass::sendViaCOMM_MODE(char* key, char* value) {
    return sendViaCOMM_MODE(key, value, false);
}

ArancinoPacket ArancinoClass::sendViaCOMM_MODE(char* key, char* value, bool isPersistent) {
	switch (COMM_MODE) {
		case SYNCH:
			return _set(key, value, isPersistent);
		break;

		case ASYNCH:
			return _publish(key, value);
		break;

		case BOTH:
			return _publish(key, value);
			return _set(key, value, isPersistent);
		break;

		default:
			return _set(key, value, isPersistent);
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
	String message = receiveArancinoResponse(END_TX_CHAR);

	return parse(message).toInt();
}*/


char** ArancinoClass::parseArray(char* data) {
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
            free(data);
        }
    }
    
    return (data != NULL && arrayParsed != NULL) ? &arrayParsed[1] : NULL;
}


int ArancinoClass::getResponseCode(char* message) {
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
      free(temp);
    }
    else
    {
        value = -1; //error
    }
    
    return value;
}


char* ArancinoClass::parse(char* message) {

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

	free(message);
	free(status);
	return value;

}

void ArancinoClass::sendArancinoCommand(char* command) {
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


void ArancinoClass::sendArancinoCommand(char command) {
    char* c = (char *)calloc(2, sizeof(char));
    c[0] = command;
    c[1] = '\0';
    sendArancinoCommand(c);
    free(c);
}


/*
 * 'terminator' char is used only for non-freeRTOS implementations.
 * For freeRTOS implementations is always used END_TX_CHAR as terminator char (see commTask()).
 */
char* ArancinoClass::receiveArancinoResponse(char terminator) {
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

bool ArancinoClass::isReservedKey(char* key) {
    int keyCount = sizeof(reservedKey) / sizeof(reservedKey[0]);
	for(int i = 0; i < keyCount; i++)
    {
		if(strcmp(reservedKey[i], key) == 0) 
			return true;
	}
	return false;
}
/*void ArancinoClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}*/

ArancinoClass Arancino;
