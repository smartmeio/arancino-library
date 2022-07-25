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
#include "ArancinoTasks.h"

ArancinoPacket reservedKeyErrorPacket = {true, RESERVED_KEY_ERROR, RESERVED_KEY_ERROR, {.string = NULL}}; //default reserved key error packet
ArancinoPacket communicationErrorPacket = {true, COMMUNICATION_ERROR, COMMUNICATION_ERROR, {.string = NULL}}; //default reserved key error packet
ArancinoPacket invalidCommandErrorPacket = {true, INVALID_VALUE_ERROR, INVALID_VALUE_ERROR, {.string = NULL}}; //default reserved key error packet

//TASK
#if defined(USEFREERTOS)
TaskHandle_t arancinoHandle1;
TaskHandle_t arancinoHandle2;
TaskHandle_t arancinoHandle3;
SemaphoreHandle_t CommMutex;
#endif

/********************************************************
					API FUNCTIONS
********************************************************/

/******** API BASIC :: ATTACH_IFACE *********/

void ArancinoClass::attachInterface(ArancinoIface& iface){
	this->_iface = &iface;
}

/******** API BASIC :: DELAY *********/
void ArancinoClass::delay(long milli){
	//Check if scheduler was started or not
#if defined(USEFREERTOS)
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED){
		long startmillis = millis();
		while(millis() - startmillis < milli);
	} else {
		vTaskDelay(milli);
	}
#else
	long startmillis = millis();
	while(millis() - startmillis < milli);
#endif
}

/******** API BASIC :: BEGIN *********/

void ArancinoClass::begin(ArancinoMetadata _amdata) {
	ArancinoConfig _acfg;
	//setting default configuration value
	begin(_amdata,_acfg);
}

void ArancinoClass::begin(ArancinoMetadata _amdata, ArancinoConfig _acfg) {
	#if defined(ARDUINO_ARCH_NRF52)
	//Patch for NRF52 as the used id is the BLE MAC rather than the micro id
	strcpy(id, getMacAddr());
	#else
	MicroID.getUniqueIDString(id, ID_SIZE/2);
	#endif
	_iface->ifaceBegin();
	arancino_id_prefix = _acfg.USE_PORT_ID_PREFIX_KEY;
	decimal_digits=_acfg.DECIMAL_DIGITS;

	_metadata = _amdata;

	int arancinocoreversionLength = 0; // assuming is not defined

	#ifdef ARANCINO_CORE_VERSION
	arancinocoreversionLength += strlen(ARANCINO_CORE_VERSION);
	#endif

	//firmware build time
	char str_build_time[strlen(__DATE__)+1+strlen(__TIME__)+1+strlen(_metadata.tzoffset)]="";
	strcpy(str_build_time, __DATE__);
	strcat(str_build_time, " ");
	strcat(str_build_time, __TIME__);
	strcat(str_build_time, " ");
	strcat(str_build_time, _metadata.tzoffset);

	char fw_lib_ver[]="FW_LIB_VER";
	char fw_name[]="FW_NAME";
	char fw_ver[]="FW_VER";
	char fw_build_time[]="FW_BUILD_TIME";
	char fw_core_ver[]="FW_CORE_VER";
	char mcu_family[]="MCU_FAMILY";
	char fw_use_freertos[]="FW_USE_FREERTOS";
	#if defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		CommMutex = xSemaphoreCreateMutex();
	}

	char* useFreeRtos = "1";
	#else
	char* useFreeRtos = "0";
	#endif
	char* keys[] = {fw_lib_ver,fw_name,fw_ver,fw_build_time,fw_core_ver, mcu_family, fw_use_freertos};
	char* values[] = {LIB_VERSION, _metadata.fwname,_metadata.fwversion,str_build_time,(char*)ARANCINO_CORE_VERSION,(char*)MCU_FAMILY,(char*)useFreeRtos};

	start(keys,values,7);
	started = true;
	
	strcpy(LOG_LEVEL,getModuleLogLevel());
	#if defined(USEFREERTOS)
	/*	All the FreeRTOS initialization should be put here, at the end of the
		the ArancinoClass::begin method.
	*/
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
	{
		CommMutex = xSemaphoreCreateMutex();
	}
	//TASK
	ArancinoTasks _atask;
	xTaskCreate(_atask.deviceIdentification, "identification", 256, NULL, ARANCINO_TASK_PRIORITY, &arancinoHandle1);
	xTaskCreate(_atask.interoception, "interoception", 256, NULL, ARANCINO_TASK_PRIORITY, &arancinoHandle2);
	xTaskCreate(_atask.sendHeartbeat, "heartbeat", 256, NULL, ARANCINO_TASK_PRIORITY, &arancinoHandle3);
	#endif
}

/******** API BASIC :: START *********/

void ArancinoClass::start(char** keys, char** values, int len) {
	ArancinoPacket packet;
	do{
		//try to start comunication every 2,5 seconds.
		delay(2500);

		packet = executeCommand(START_COMMAND, NULL, keys, values,NULL, len, false, STRING_ARRAY);
		if(packet.responseCode == RSP_OK){
			//timestamp from arancino module
			memcpy(timestamp, packet.response.stringArray[1],13);
			char tmst_part[5];
			memcpy(tmst_part,timestamp, 4);  //first 4 digits of epoch
			tmst_sup=atoi(tmst_part);
			tmst_inf=atoi(&timestamp[4]);  //last 9 digits of epoch
			millis_previous=millis();
		}
	}while (packet.isError == true || packet.responseCode != RSP_OK);
	free(packet);
}

#if  defined(USEFREERTOS)
/******** API ADVANCED :: START SCHEDULER *********/

void ArancinoClass::startScheduler() {
#if defined(__SAMD21G18A__)
	vSetErrorLed(LED_BUILTIN, HIGH);
#endif
	/*
	 * Uncomment this if you want run loop() as a dedicated task.
	 * If loop() doesn't run as dedicated task, should not contain blocking code.
	 */
	//task started in main.c (core)
	#if !defined (ARDUINO_ARANCINO_VOLANTE)
	vTaskStartScheduler();
	#endif
}
#endif

/******** API ADVANCED :: DEBUG OPTIONS *********/

void ArancinoClass::enableDebugMessages(bool sendViaCommMode){
	if (sendViaCommMode){
		_isDebug = true;
		_commMode = true;
	} else {
		//Default config for Arancino Boards
		#if defined(SERIAL_DEBUG) && defined(BAUDRATE_DEBUG)
		SERIAL_DEBUG.begin(BAUDRATE_DEBUG);
		_isDebug = true;
		_dbgSerial = &SERIAL_DEBUG;
		#endif
	}
}

void ArancinoClass::enableDebugMessages(Stream& dbgSerial){
	//Custom debug serial should be provided here and already should be already initialized
	if (dbgSerial != NULL){
		_isDebug = true;
		this->_dbgSerial = &dbgSerial;
	}
}

void ArancinoClass::disableDebugMessages(){
	//This only stops debug messages from being processed. If you want to reuse the dedicated
	//serial port at a different baud rate you should manually end the serial yourself
	_isDebug = false;
	_dbgSerial = NULL;
}

/******** API BASIC :: MSET *********/

ArancinoPacket ArancinoClass::mset(char** keys, char** values, int len, bool isPersistent) {
	if ((keys == NULL) || (values == NULL) || (len <= 0))
		return invalidCommandErrorPacket;
	if(isPersistent)
		return executeCommand(MSET_PERS_COMMAND, NULL, keys, values,NULL, len, arancino_id_prefix, VOID);
	else
		return executeCommand(MSET_COMMAND, NULL, keys, values,NULL, len, arancino_id_prefix, VOID);
}

/******** API BASIC :: MGET *********/

template<> ArancinoPacket ArancinoClass::mget<ArancinoPacket>(char** keys, int len){
	if ((keys == NULL) || (len <= 0))
		return invalidCommandErrorPacket;
	return executeCommand(MGET_COMMAND, NULL, keys, NULL, NULL, len, arancino_id_prefix, STRING_ARRAY);
}

template<> char** ArancinoClass::mget(char** keys, int len) {
	ArancinoPacket packet = mget<ArancinoPacket>(keys, len);
	if (packet.isError)
		return NULL;
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
	if(key == NULL && value == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	if(isPersistent)
		return executeCommand(SET_PERS_COMMAND, key, value, NULL, arancino_id_prefix, VOID);
	else
		return executeCommand(SET_COMMAND, key, value, NULL , arancino_id_prefix, VOID);
}

/******** API BASIC :: GET *********/

template<> ArancinoPacket ArancinoClass::get<ArancinoPacket>(char* key){
	if(key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	char* param = NULL;
	return executeCommand(GET_COMMAND,key,param,param,arancino_id_prefix,STRING);
}

template<> char* ArancinoClass::get(char* key){
	ArancinoPacket packet = get<ArancinoPacket>(key);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

/******** API BASIC :: SETRESERVED *********/

ArancinoPacket ArancinoClass::setReserved( char* key, char* value, bool id_prefix) {
	if(key == NULL && value == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	return executeCommand(SETRESERVED_COMMAND,key,value,NULL,id_prefix,VOID);
}

/******** API BASIC :: GETRESERVED *********/

template<> ArancinoPacket ArancinoClass::getReserved<ArancinoPacket>(char* key, bool id_prefix){
	if(key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	return executeCommand(GETRESERVED_COMMAND,key,NULL,NULL,id_prefix,STRING);
}

template<> char* ArancinoClass::getReserved(char* key, bool id_prefix){
	ArancinoPacket packet = getReserved<ArancinoPacket>(key, id_prefix);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

char* ArancinoClass::getModuleVersion(){
	char key[strlen(MODVERS_KEY)+1];
	strcpy(key,MODVERS_KEY);
	char* retString = getReserved(key, false);
	return retString;
}

char* ArancinoClass::getModuleLogLevel(){
	char key[strlen(MODLOGLVL_KEY)+1];
	strcpy(key,MODLOGLVL_KEY);
	char* retString = getReserved(key, false);
	return retString;
}

char* ArancinoClass::getModuleEnvironment(){
	char key[strlen(MODENV_KEY)+1];
	strcpy(key,MODENV_KEY);
	char* retString = getReserved(key, false);
	return retString;
}

char* ArancinoClass::getBlinkId(){
	char key[strlen(BLINK_ID_KEY)+1];
	strcpy(key,BLINK_ID_KEY);
	char* retString = getReserved(key, true);
	return retString;
}

/******** API BASIC :: SETRESERVED *********/

ArancinoPacket ArancinoClass::setBlinkId(int value){
	char str[20] = "";
	itoa(value, str, 10);
	return setReserved(BLINK_ID_KEY,str, true);
}

/******** API BASIC :: DEL *********/

template<> ArancinoPacket ArancinoClass::del<ArancinoPacket> (char* key){
	if(key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	return executeCommand(DEL_COMMAND,key,NULL,NULL,arancino_id_prefix,INT);
}

template<> int ArancinoClass::del(char* key){
	ArancinoPacket packet = del<ArancinoPacket>(key);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

/******** API BASIC :: HSET *********/

ArancinoPacket ArancinoClass::hset( char* key, char* field, int value, bool isPersistent) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, float value, bool isPersistent) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, double value, bool isPersistent) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, uint32_t value, bool isPersistent) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field, long value, bool isPersistent) {
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset( char* key, char* field , char* value, bool isPersistent) {
	if(key == NULL && field == NULL && value == NULL && strcmp(key, "") == 0 && strcmp(field, "") == 0)
		return invalidCommandErrorPacket;
	if(isPersistent)
		return executeCommand(HSET_PERS_COMMAND,key,field,value,arancino_id_prefix,VOID);
	else
		return executeCommand(HSET_COMMAND,key,field,value,arancino_id_prefix,VOID);
}

/******** API BASIC :: HGET *********/

template<> ArancinoPacket ArancinoClass::hget<ArancinoPacket> (char* key, char* field){
	if(key == NULL && field == NULL && strcmp(key, "") == 0 && strcmp(field, "") == 0 )
		return invalidCommandErrorPacket;
	return executeCommand(HGET_COMMAND,key,field,NULL,arancino_id_prefix,STRING);
}

template<> char* ArancinoClass::hget(char* key, char* field){
	ArancinoPacket packet = hget<ArancinoPacket>(key, field);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

/******** API BASIC :: HGETALL PACKET *********/

template<> ArancinoPacket ArancinoClass::hgetall<ArancinoPacket> (char* key){
	if(key == NULL && strcmp(key, "") == 0 )
		return invalidCommandErrorPacket;
	char* param = NULL;
	return executeCommand(HGETALL_COMMAND,key,param,param,arancino_id_prefix,STRING_ARRAY);
}

template<> char** ArancinoClass::hgetall(char* key){
	ArancinoPacket packet = hgetall<ArancinoPacket>(key);
	if (!packet.isError)
		return packet.response.stringArray;
	else
		return NULL;
}

/******** API BASIC :: HKEYS *********/

template<> ArancinoPacket ArancinoClass::hkeys<ArancinoPacket> (char* key){
	if(key == NULL && strcmp(key, "") == 0 )
		return invalidCommandErrorPacket;
	char* param = NULL;
	return executeCommand(HKEYS_COMMAND,key,param,param,arancino_id_prefix,STRING_ARRAY);
}

template<> char** ArancinoClass::hkeys(char* key){
	ArancinoPacket packet = hkeys<ArancinoPacket>(key);
	char** retArray;
	if (!packet.isError)
		retArray = packet.response.stringArray;
	else
		retArray = NULL;
	return retArray;
}

/******** API BASIC :: HVALS *********/

template<> ArancinoPacket ArancinoClass::hvals<ArancinoPacket> (char* key){
	if(key == NULL && strcmp(key, "") == 0 )
		return invalidCommandErrorPacket;
	char* param = NULL;
	return executeCommand(HVALS_COMMAND,key,param,param,arancino_id_prefix,STRING_ARRAY);
}

template<> char** ArancinoClass::hvals(char* key){
	ArancinoPacket packet = hvals<ArancinoPacket>(key);
	char** retArray;
	if (!packet.isError)
		retArray = packet.response.stringArray;
	else
		retArray = NULL;
	return retArray;
}

/******** API BASIC :: HDEL *********/

template<> ArancinoPacket ArancinoClass::hdel<ArancinoPacket> (char* key, char* field){
	if(key == NULL && field == NULL && strcmp(key,"") == 0 && strcmp(field, "") == 0 )
		return invalidCommandErrorPacket;
	return executeCommand(HDEL_COMMAND,key,field,NULL,arancino_id_prefix,INT);
}

template<> int ArancinoClass::hdel(char* key, char* field){
	ArancinoPacket packet = hdel<ArancinoPacket>(key, field);
	int retValue = 0;
	if (!packet.isError)
		retValue = packet.response.integer;
	return retValue;
}

/******** API BASIC :: KEYS PACKET *********/

template<> ArancinoPacket ArancinoClass::keys<ArancinoPacket> (char* pattern){
	if(pattern == NULL && strcmp(pattern,"") == 0)
		return invalidCommandErrorPacket;
	char* param = NULL;
	return executeCommand(KEYS_COMMAND,pattern,param,param,false,STRING_ARRAY);
}

template<> char** ArancinoClass::keys(char* pattern){
	ArancinoPacket packet = keys<ArancinoPacket>(pattern);
	if (!packet.isError)
		return packet.response.stringArray;
	else
		return NULL;
}

/******** API BASIC :: PUBLISH *********/

ArancinoPacket ArancinoClass::__publish (char* channel, char* msg) {
	if(channel == NULL && msg == NULL && strcmp(channel,"") == 0 )
		return invalidCommandErrorPacket;
	return executeCommand(PUBLISH_COMMAND,channel,msg,NULL,arancino_id_prefix,INT);
}

template<> ArancinoPacket ArancinoClass::publish<ArancinoPacket> (char* channel, char* msg){
	return __publish(channel, msg);
}

template<> int ArancinoClass::publish(char* channel, char* msg){
	ArancinoPacket packet = __publish(channel, msg);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template<> ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char* channel, double msg){
	char str[20] = "";
	_doubleToString(msg, decimal_digits, str);
	return __publish(channel, str);
}

template<> int ArancinoClass::publish(char* channel, double msg){
	char str[20] = "";
	_doubleToString(msg, decimal_digits, str);
	ArancinoPacket packet = __publish(channel, str);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template<> ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char* channel, int msg){
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str);
}

template<> int ArancinoClass::publish(char* channel, int msg){
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template<> ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char* channel, uint32_t msg){
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str);
}

template<> int ArancinoClass::publish(char* channel, uint32_t msg){
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template<> ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char* channel, long msg){
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str);
}

template<> int ArancinoClass::publish(char* channel, long msg){
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

/******** API BASIC :: FLUSH *********/

ArancinoPacket ArancinoClass::flush() {
	char* param = NULL;
	return executeCommand(FLUSH_COMMAND,param,param,param,false,VOID);
}

/******** API BASIC :: STORE *********/

template<> ArancinoPacket ArancinoClass::store<ArancinoPacket>( char* key, int value) {
	char str[20] = "";
	itoa(value, str, 10);
	return __store(key, str);
}

template<> char* ArancinoClass::store(char* key, int value){
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet = __store(key, str);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template<> ArancinoPacket ArancinoClass::store<ArancinoPacket>( char* key, uint32_t value) {
	char str[20] = "";
	itoa(value, str, 10);
	return __store(key, str);
}

template<> char* ArancinoClass::store(char* key, uint32_t value){
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet = __store(key, str);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template<> ArancinoPacket ArancinoClass::store<ArancinoPacket>( char* key, long value) {
	char str[20] = "";
	itoa(value, str, 10);
	return __store(key, str);
}

template<> char* ArancinoClass::store(char* key, long value){
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet = __store(key, str);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template<> ArancinoPacket ArancinoClass::store<ArancinoPacket>( char* key, float value) {
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return __store(key, str);
}

template<> char* ArancinoClass::store(char* key, float value){
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	ArancinoPacket packet = __store(key, str);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template<> ArancinoPacket ArancinoClass::store<ArancinoPacket>( char* key, double value) {
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return __store(key, str);
}

template<> char* ArancinoClass::store(char* key, double value){
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	ArancinoPacket packet = __store(key, str);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

ArancinoPacket ArancinoClass::__store( char* key, char* value) {
	char* ts = getTimestamp();
	if(key == NULL && value == NULL && strcmp(key, "") == 0){
		return invalidCommandErrorPacket;
	}
	return executeCommand(STORE_COMMAND,key,value,ts,true,STRING);
}

/******** API BASIC :: MSTORE *********/

template<> ArancinoPacket ArancinoClass::mstore<ArancinoPacket> (char** keys, char** values, int len) {
	char* ts = getTimestamp();
	if ((keys == NULL) || (values == NULL) || (len <= 0)) {
		return invalidCommandErrorPacket;
	}
	return executeCommand(MSTORE_COMMAND, NULL, keys, values, ts, len, true, STRING_ARRAY);
}

template<> char** ArancinoClass::mstore(char** key, char** value, int len){
	ArancinoPacket packet = mstore<ArancinoPacket>(key, value, len);
	if (!packet.isError)
		return packet.response.stringArray;
	else
		return NULL;
}

/******** API BASIC :: STORETAGS *********/

ArancinoPacket ArancinoClass::storetags(char* key, char** tags, char** values, int len) {
	char* ts = getTimestamp();
	if ((key == NULL) || (tags == NULL) || (values == NULL) || (len <= 0))
		return invalidCommandErrorPacket;
	return executeCommand(STORETAGS_COMMAND,key,tags,values,ts,len,true,VOID);
}

/******** API UTILITY :: FREE *********/

void ArancinoClass::free(char* str){
	#if defined(USEFREERTOS)
	vPortFree(str);
	#else
	std::free(str);
	#endif
}

void ArancinoClass::free(char** _array){
	char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;

	if (_array != NULL){
		#if defined(USEFREERTOS)
		vPortFree(*_array);
		#else
		std::free(*_array);
		#endif
	}
	if (dummy != NULL){
		#if defined(USEFREERTOS)
		vPortFree(dummy);
		#else
		std::free(dummy);
		#endif
	}
}

void ArancinoClass::free(ArancinoPacket packet){
	if (packet.responseType == STRING)
	{
		#if defined(USEFREERTOS)
		vPortFree(packet.response.string);
		#else
		std::free(packet.response.string);
		#endif
	}
	else if (packet.responseType == STRING_ARRAY)
	{
		free(packet.response.stringArray);
	}
}

void * ArancinoClass::malloc(size_t _size)
{
    /* Call the FreeRTOS version of malloc. */
	#if defined(USEFREERTOS)
	return pvPortMalloc(_size);
	#else
	return std::malloc(_size);
	#endif
}

void * ArancinoClass::calloc (size_t nmemb, size_t _size)
{
    /* Call the FreeRTOS version of malloc. */
	#if defined(USEFREERTOS)
		#if defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2) || defined (ARDUINO_ARANCINO_VOLANTE)
		uint8_t *ptr=(uint8_t *)malloc(nmemb*(_size));
		memset(ptr,0,nmemb);//clear the buffer #pte4c0
		return ptr;
        	#else
		return pvPortCalloc(nmemb, _size);
		#endif
	#else
	return std::calloc(nmemb, _size);
	#endif
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
	_printDebugMessage(value);
}

void ArancinoClass::print(String value){
	_printDebugMessage(value.begin());
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
	char ts_sup_tmp[5];
	sprintf(ts_sup_tmp, "%04ld", tmst_sup); //4 digits
	memcpy(timestamp,ts_sup_tmp,4);
	char ts_inf_tmp[10];
	sprintf(ts_inf_tmp, "%09ld", tmst_inf); //9 digits
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

ArancinoPacket ArancinoClass::executeCommand(char* command, char* param1, char** params2, char** params3, char* param4, int len, bool id_prefix, int type_return){
	int commandLength = strlen(command);
	int strLength = commandLength + 1;
	int param1_length = 0;
	int param4_length = 0;

	if(param1 != NULL){
		param1_length = strlen(param1);
		if(id_prefix){
			param1_length += ID_SIZE + 1;
		}
		strLength += param1_length + 1;
	}

	// Calculating Cortex Protocol command length
	for (int i = 0; i < len; i ++) {
		char* param2 = params2[i];
		char* param3 = NULL;
		if(params3 != NULL)
			param3 = params3[i];

		int param2_length = strlen(param2);
		if(id_prefix && param1 == NULL){
			param2_length += ID_SIZE + 1;
		}
		int param3_length = 0;
		if(params3 != NULL)
			param3_length = strlen(param3);
		// For every key-value pair the length of both tag and value is added
		// two 1s are added in order to take into account the % chr and
		// the # and @ chrs in the case of last tag or last value respectively
		strLength += param2_length + 1;
		if(params3 != NULL)
			strLength +=param3_length + 1;
	}

	if(param4 != NULL){
		param4_length = strlen(param4);
		strLength += param4_length + 1;
	}

	char* str = (char*) calloc(strLength, sizeof(char));

	strcpy(str, command);
	if(param1 != NULL){
		strcat(str, dataSplitStr);
		if(id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, param1);
	}
	strcat(str, dataSplitStr);
	// Points to the memory area where tags have to be written
	char* params2Pointer = str + commandLength + strlen(dataSplitStr);
	if(param1 != NULL)
		params2Pointer += param1_length + strlen(dataSplitStr);

	// Points at the end of the string less the space reserved to timestamp
	char* params3Pointer = NULL;
	if(params3 != NULL)
		params3Pointer = str + strLength;
	if(param4 != NULL)
		params3Pointer = params3Pointer - param4_length - strlen(endTXStr);

	// The string to send is built in 1 single loop.
	// keys are copied from first to last (left to right in string)
	// and values are copied from last to first (right to left in string)
	for(int i = 0; i < len; i ++) {
		char* param2 = params2[i];
		char* param3 = NULL;
		if(params3 != NULL)
			param3 = params3[len - (i + 1)];

		if(id_prefix && param1 == NULL){
			strcat(params2Pointer, id);
			strcat(params2Pointer, ID_SEPARATOR);
		}
		strcat(params2Pointer, param2);

		if (i == len - 1) { // If it's the last key we have to use #(\4) instead of %(\16)
			if(params3 != NULL)
				strcat(params2Pointer, dataSplitStr);
		} else {
			strcat(params2Pointer, arraySplitStr);
		}

		// We use memcpy rather than strcat here because it would append \0,
		// thus terminating the string prematurely
		if(params3 != NULL){
			params3Pointer -= strlen(param3) + 1;

			memcpy(params3Pointer, param3, strlen(param3));

			if (i != 0) {
				memcpy(params3Pointer + strlen(param3), arraySplitStr, 1);
			}
		}

	}
	//timestamp
	if(param4 != NULL){
		strcat(str, dataSplitStr);
		strcat(str, param4);
	}
	strcat(str, endTXStr);

	char *message = NULL;
	if (takeCommMutex((TickType_t)portMAX_DELAY) != pdFALSE)
	{
		_iface->sendArancinoCommand(str);
		message = _iface->receiveArancinoResponse(END_TX_CHAR);
		giveCommMutex();
	}

	free(str);

	if (message == NULL)
		return communicationErrorPacket;

	ArancinoPacket packet = createArancinoPacket(message, type_return);

	free(message);

	return packet;
}

ArancinoPacket ArancinoClass::executeCommand(char* command, char* param1, char* param2, char* param3, bool id_prefix, int type_return){
	int commandLength = strlen(command);
	int param1_length = 0;
	if(param1 != NULL){
		param1_length = strlen(param1);
		if(id_prefix){
			param1_length += ID_SIZE + 1;
		}
	}
	int param2_length = 0;
	if(param2 != NULL)
		param2_length=strlen(param2);
	int param3_length = 0;
	if(param3 != NULL)
		param3_length=strlen(param3);
	int strLength = commandLength + 1 + param1_length + 1 + param2_length + 1 + param3_length + 1 + 1;

	char* str = (char *)calloc(strLength, sizeof(char));

	strcpy(str, command);
	if(param1 != NULL){
		strcat(str, dataSplitStr);
		if(id_prefix){
			strcat(str, id);
			strcat(str, ID_SEPARATOR);
		}
		strcat(str, param1);
	}
	if(param2 != NULL){
		strcat(str, dataSplitStr);
		strcat(str, param2);
	}
	if(param3 != NULL){
		strcat(str, dataSplitStr);
		strcat(str, param3);
	}
	strcat(str, endTXStr);

	char *message = NULL;
	if (takeCommMutex((TickType_t)portMAX_DELAY) != pdFALSE)
	{
		_iface->sendArancinoCommand(str);
		message = _iface->receiveArancinoResponse(END_TX_CHAR);
		giveCommMutex();
	}

	free(str);

	//parse response
	ArancinoPacket packet = createArancinoPacket(message, type_return);

	free(message);

	return packet;

}

ArancinoPacket ArancinoClass::createArancinoPacket(char* message, int type_return){
	ArancinoPacket packet;
	if (message == NULL)
		return communicationErrorPacket;

	if(type_return == VOID){
		ArancinoPacket temp = {false, _getResponseCode(message), VOID, {.string = NULL}};
		packet = temp;
	}
	else if(type_return == INT){
		char* messageParsed = _parse(message);
		ArancinoPacket temp = {false, _getResponseCode(message), INT, {.integer = atoi(messageParsed)}};
		free(messageParsed);
		packet = temp;
	}
	else if (type_return == STRING){
		ArancinoPacket temp = {false, _getResponseCode(message), STRING, {.string = _parse(message)}};
		packet = temp;
	}
	else if (type_return == STRING_ARRAY){
		ArancinoPacket temp = {false, _getResponseCode(message), STRING_ARRAY, {.stringArray = _parseArray(_parse(message))}};
		packet = temp;
	}
	else{
		ArancinoPacket temp = invalidCommandErrorPacket;
		packet = temp;
	}
	return packet;
}

void ArancinoClass::systemReset(){
	#if defined(ARDUINO_ARCH_RP2040)
	watchdog_reboot(0,0,0);
	#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_NRF52)
	NVIC_SystemReset();
	#else 
	#warning "Unsupported board selected. Please make sure to provide a reset implementation inside Arancino.cpp otherwise non-serial communication may not work as intended"
	#endif
}

void ArancinoClass::_doubleToString(double value, unsigned int _nDecimal, char* str) {
	noInterrupts();
	char val[20]="";
	sprintf(val, "%d", int(value));
	int valueLength=strlen(val);
	if(valueLength+_nDecimal>15){			//The double data type has 15 decimal digits of precision
      _nDecimal=15-valueLength;
      if(_nDecimal<0){
        _nDecimal=0;
      }
  	}
	double rounding = 0.5;
	rounding /=pow(10,_nDecimal);
	int decimalPart = pow(10,_nDecimal) * (value - int(value) + rounding);
	char dec[2];
	itoa(_nDecimal,dec,10);
	char param[10];
	strcpy(param,"%d.%0");
	strcat(param,dec);
  	strcat(param,"d");
	sprintf(str,param, int(value), decimalPart);
	interrupts();
	//return str;
	//dtostrf(value, valueLength, _nDecimal, str);
}

void ArancinoClass::_floatToString(float value, unsigned int _nDecimal, char* str) {
	noInterrupts();
	char val[20]="";
	sprintf(val, "%d", int(value));
	int valueLength=strlen(val);
	if(valueLength+_nDecimal>7){			//The float data type has 7 decimal digits of precision
      _nDecimal=7-valueLength;
      if(_nDecimal<0){
        _nDecimal=0;
      }
  	}
	double rounding = 0.5;
	rounding /=pow(10,_nDecimal);
	int decimalPart = pow(10,_nDecimal) * (value - int(value) + rounding);
	char dec[2];
	itoa(_nDecimal,dec,10);
	char param[10];
	strcpy(param,"%d.%0");
	strcat(param,dec);
  	strcat(param,"d");
	sprintf (str,param, int(value), decimalPart);
	interrupts();
	//return str;
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

void ArancinoClass::_printDebugMessage(char* value) {
	if (_isDebug && _commMode){
	__publish(MONITOR_KEY, value);
	__set(MONITOR_KEY, value);
	} else if(_isDebug && _dbgSerial != NULL){
		_dbgSerial->print(value);
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
	  free(temp);
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

	free(status);

	if (strcmp(value, nullStr) == 0){
		free(value);
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
			else if (int(strlen(previousDSCIndex)) > maxLength)
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
			free(data);
		}
	}

	return (data != NULL && arrayParsed != NULL) ? &arrayParsed[1] : NULL;
}

BaseType_t ArancinoClass::takeCommMutex(TickType_t timeout)
{
#if defined(USEFREERTOS)

	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if (CommMutex != NULL)
		{
			return xSemaphoreTake(CommMutex, timeout);
		}
		else
		{
			return pdFALSE;
		}
	}
	else
	{
		/*
		Since the scheduler is not yet started, there are no multiple tasks
		running simultaneously, so takeCommMutex acts as the mutex is correctly
		taken.
		*/
		return pdTRUE;
	}
#else
	return pdTRUE;
#endif
}

void ArancinoClass::giveCommMutex()
{
#if defined(USEFREERTOS)
	if (CommMutex == NULL)
	{
		return;
	}
	else if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xSemaphoreGive(CommMutex);
	}
#endif
}


void ArancinoClass::taskSuspend(){
	#if  defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		vTaskSuspendAll();
	}
	#endif
}

void ArancinoClass::taskResume(){
	#if  defined(USEFREERTOS)
	if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
		xTaskResumeAll();
	}
	#endif
}

ArancinoClass Arancino;
