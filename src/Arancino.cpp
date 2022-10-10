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

#define DEBUG 0

ArancinoPacket reservedKeyErrorPacket = {true, RESERVED_KEY_ERROR, RESERVED_KEY_ERROR, {.string = NULL}};	   // default reserved key error packet
ArancinoPacket communicationErrorPacket = {true, COMMUNICATION_ERROR, COMMUNICATION_ERROR, {.string = NULL}};  // default reserved key error packet
ArancinoPacket invalidCommandErrorPacket = {true, INVALID_VALUE_ERROR, INVALID_VALUE_ERROR, {.string = NULL}}; // default reserved key error packet

// TASK
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

void ArancinoClass::begin(ArancinoMetadata _amdata)
{
	ArancinoConfig _acfg;
	// setting default configuration value
	begin(_amdata, _acfg);
}

void ArancinoClass::begin(ArancinoMetadata _amdata, ArancinoConfig _acfg) {

	MicroID.getUniqueIDString(id, ID_SIZE/2);
	_iface->ifaceBegin();
	arancino_id_prefix = _acfg.USE_PORT_ID_PREFIX_KEY;
	decimal_digits = _acfg.DECIMAL_DIGITS;

	_metadata = _amdata;

	int arancinocoreversionLength = 0; // assuming is not defined

	#ifdef ARANCINO_CORE_VERSION
	arancinocoreversionLength += strlen(ARANCINO_CORE_VERSION);
	#endif

	// firmware build time
	char str_build_time[strlen(__DATE__) + 1 + strlen(__TIME__) + 1 + strlen(_metadata.tzoffset)] = "";
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

	/*
	char* keys[] = {fw_lib_ver,fw_name,fw_ver,fw_build_time,fw_core_ver, mcu_family, fw_use_freertos};
	char* values[] = {LIB_VERSION, _metadata.fwname,_metadata.fwversion,str_build_time,(char*)ARANCINO_CORE_VERSION,(char*)MCU_FAMILY,(char*)useFreeRtos};

	start(keys,values,7);
	started = true;
	*/
	
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

void ArancinoClass::start(char **keys, char **values, int len)
{
}

#if defined(USEFREERTOS)
/******** API ADVANCED :: START SCHEDULER *********/

void ArancinoClass::startScheduler()
{
	started = true;
#if defined(__SAMD21G18A__)
	vSetErrorLed(LED_BUILTIN, HIGH);
#endif
	/*
	 * Uncomment this if you want run loop() as a dedicated task.
	 * If loop() doesn't run as dedicated task, should not contain blocking code.
	 */
	// task started in main.c (core)
#if defined(ARDUINO_ARANCINO_VOLANTE)
	// initYieldTask(100);
#elif defined(ARDUINO_ARCH_RP2040)
	// initYieldTask(100);
	// runLoopAsTask(128, tskIDLE_PRIORITY);
	// initFreeRTOS(); //128 = stack depth for loop, tskIDLE_PRIORITY = priority
#elif defined(__SAMD21G18A__)
	//runLoopAsTask(128, tskIDLE_PRIORITY);
	vTaskStartScheduler();
#elif defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2)
	vTaskStartScheduler();
#else
#error "FreeRTOS not supported on the selected board!"
#endif
}
#endif

/******** API BASIC :: DEBUG OPTIONS *********/

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
	if (_dbgSerial != NULL){
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


/******** API BASIC :: SET *********/

ArancinoPacket ArancinoClass::set(char *key, int value, bool isAck, bool isPersistent, char* type)
{
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isAck, isPersistent);
}

ArancinoPacket ArancinoClass::set(char *key, uint32_t value, bool isAck, bool isPersistent, char* type)
{
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isAck, isPersistent);
}

ArancinoPacket ArancinoClass::set(char *key, long value, bool isAck, bool isPersistent, char* type)
{
	char str[20] = "";
	itoa(value, str, 10);
	return set(key, str, isAck, isPersistent);
}

ArancinoPacket ArancinoClass::set(char *key, float value, bool isAck, bool isPersistent, char* type)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return set(key, str, isAck, isPersistent);
}

ArancinoPacket ArancinoClass::set(char *key, double value, bool isAck, bool isPersistent, char* type)
{
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return set(key, str, isAck, isPersistent);
}

ArancinoPacket ArancinoClass::set(char *key, char *value, bool isAck, bool isPersistent, char* type)
{
	if (key == NULL || value == NULL || strcmp(key, "") == 0)
		return invalidCommandErrorPacket;

	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(SET_COMMAND, key, NULL, value, true, true, cfg, VOID_RESPONSE);
}


/******** API BASIC :: MSET *********/

ArancinoPacket ArancinoClass::mset(char **keys, char **values, int len, bool isPersistent, char* type )
{
	if ((keys == NULL) || (values == NULL) || (len <= 0))
		return invalidCommandErrorPacket;
	
	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(SET_COMMAND, keys, NULL, values, len, true, true, cfg, VOID_RESPONSE);
}

/******** API BASIC :: GET *********/

template <>
ArancinoPacket ArancinoClass::get<ArancinoPacket>(char* key, bool isPersistent, char* type)
{
	if (key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	
	ArancinoCFG cfg;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(GET_COMMAND, key, NULL, NULL, true, false, cfg, KEY_VALUE_RESPONSE);
}

template <> char *ArancinoClass::get(char* key, bool isPersistent, char* type)
{
	ArancinoPacket packet = get<ArancinoPacket>(key, isPersistent, type);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

/******** API BASIC :: MGET *********/

template <> ArancinoPacket ArancinoClass::mget<ArancinoPacket>(char **keys, int len, bool isPersistent, const char* type)
{
	if ((keys == NULL) || (len <= 0))
		return invalidCommandErrorPacket;

	ArancinoCFG cfg;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(GET_COMMAND, keys, NULL, NULL, len, true, false, cfg, KEY_VALUE_RESPONSE);
}

template <> char **ArancinoClass::mget(char** keys, int len, bool isPersistent, const char* type)
{
	ArancinoPacket packet = mget<ArancinoPacket>(keys, len, isPersistent, type);
	if (packet.isError)
		return NULL;
	return packet.response.stringArray;
}

/******** API BASIC :: DEL *********/

template <>
ArancinoPacket ArancinoClass::del<ArancinoPacket>(char* key, bool isAck, bool isPersistent, const char* type)
{
	if (key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	
	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(DEL_COMMAND, key, NULL, NULL, true, false, cfg, KEY_RESPONSE);
}

template <>
int ArancinoClass::del(char* key, bool isAck, bool isPersistent, const char* type)
{
	ArancinoPacket packet = del<ArancinoPacket>(key, isAck, isPersistent, type);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

/******** API BASIC :: HSET *********/

ArancinoPacket ArancinoClass::hset(char *key, char *field, int value, bool isAck, bool isPersistent, const char* type)
{
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str, isAck, isPersistent, type);
}

ArancinoPacket ArancinoClass::hset(char *key, char *field, float value, bool isAck, bool isPersistent, const char* type)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return hset(key, field, str);
}

ArancinoPacket ArancinoClass::hset(char *key, char *field, double value, bool isAck, bool isPersistent, const char* type)
{
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	return hset(key, field, str, isAck, isPersistent, type);
}

ArancinoPacket ArancinoClass::hset(char *key, char *field, uint32_t value, bool isAck, bool isPersistent, const char* type)
{
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str, isAck, isPersistent, type);
}

ArancinoPacket ArancinoClass::hset(char *key, char *field, long value, bool isAck, bool isPersistent, const char* type)
{
	char str[20];
	itoa(value, str, 10);
	return hset(key, field, str, isAck, isPersistent, type);
}

ArancinoPacket ArancinoClass::hset(char *key, char *field, char *value, bool isAck, bool isPersistent, const char* type)
{
	if (key == NULL && field == NULL || value == NULL || strcmp(key, "") == 0 || strcmp(field, "") == 0)
		return invalidCommandErrorPacket;
	
	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(HSET_COMMAND, key, field, value, true, true, cfg, VOID_RESPONSE);
}

/******** API BASIC :: HGET *********/

template <>
ArancinoPacket ArancinoClass::hget<ArancinoPacket>(char *key, char *field, bool isPersistent, const char* type)
{
	if (key == NULL || field == NULL || strcmp(key, "") == 0 || strcmp(field, "") == 0)
		return invalidCommandErrorPacket;

	ArancinoCFG cfg;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	cfg.type = type;
	return executeCommand(HGET_COMMAND, key, field, NULL, true, true, cfg, KEY_VALUE_RESPONSE);
}

template <>
char *ArancinoClass::hget(char *key, char *field, bool isPersistent, const char* type)
{
	ArancinoPacket packet = hget<ArancinoPacket>(key, field, isPersistent, type);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

/******** API BASIC :: HDEL *********/

template <>
ArancinoPacket ArancinoClass::hdel<ArancinoPacket>(char *key, char *field, bool isAck, bool isPersistent)
{
	if (key == NULL && field == NULL && strcmp(key, "") == 0 && strcmp(field, "") == 0)
		return invalidCommandErrorPacket;
	
	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.pers = isPersistent ? CFG_TRUE : CFG_FALSE;
	return executeCommand(HDEL_COMMAND, key, field, NULL, true, true, cfg, FIELDS_RESPONSE);
}

template <>
int ArancinoClass::hdel(char *key, char *field, bool isAck, bool isPersistent)
{
	ArancinoPacket packet = hdel<ArancinoPacket>(key, field, isAck, isPersistent);
	int retValue = 0;
	if (!packet.isError)
		retValue = packet.response.integer;
	return retValue;
}

/******** API BASIC :: PUBLISH *********/

template <>
ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char *channel, char *msg, bool isAck)
{
	return __publish(channel, msg, isAck);
}

template <>
int ArancinoClass::publish(char *channel, char *msg, bool isAck)
{
	ArancinoPacket packet = __publish(channel, msg, isAck);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template <>
ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char *channel, double msg, bool isAck)
{
	char str[20] = "";
	_doubleToString(msg, decimal_digits, str);
	return __publish(channel, str, isAck);
}

template <>
int ArancinoClass::publish(char *channel, double msg, bool isAck)
{
	char str[20] = "";
	_doubleToString(msg, decimal_digits, str);
	ArancinoPacket packet = __publish(channel, str, isAck);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template <>
ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char *channel, int msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str, isAck);
}

template <>
int ArancinoClass::publish(char *channel, int msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str, isAck);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template <>
ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char *channel, uint32_t msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str, isAck);
}

template <>
int ArancinoClass::publish(char *channel, uint32_t msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str, isAck);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

template <>
ArancinoPacket ArancinoClass::publish<ArancinoPacket>(char *channel, long msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	return __publish(channel, str, isAck);
}

template <>
int ArancinoClass::publish(char *channel, long msg, bool isAck)
{
	char str[20] = "";
	itoa(msg, str, 10);
	ArancinoPacket packet = __publish(channel, str, isAck);
	if (!packet.isError)
		return packet.response.integer;
	else
		return 0;
}

ArancinoPacket ArancinoClass::__publish(char *channel, char *msg, bool isAck)
{
	if (channel == NULL || msg == NULL || strcmp(channel, "") == 0)
		return invalidCommandErrorPacket;

	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	JsonObject cmd_args = cmd_doc.createNestedObject("args");
	cmd_doc["cmd"] = PUBLISH_COMMAND;
	JsonArray cmd_items = cmd_args.createNestedArray("items");
	JsonObject items_obj = cmd_items.createNestedObject();
	items_obj["channel"] = channel;
	items_obj["message"] = msg;

	JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
	cmd_cfg["ack"] = isAck? 0 : 1;
	return executeCommand(cmd_doc, CLIENTS_RESPONSE);
}

/******** API BASIC :: FLUSH *********/

ArancinoPacket ArancinoClass::flush(bool isAck, bool isPersistent)
{
	ArancinoCFG cfg;
	cfg.ack = isAck ? CFG_TRUE : CFG_FALSE;
	cfg.persistent = isPersistent ? CFG_TRUE : CFG_FALSE;
	return executeCommand(FLUSH_COMMAND, NULL, NULL, NULL, false, false, cfg, VOID_RESPONSE);
}

/******** API BASIC :: STORE *********/

template <>
ArancinoPacket ArancinoClass::store<ArancinoPacket>(char *key, int value, char* timestamp, bool isAck,)
{
	char str[20] = "";
	itoa(value, str, 10);
	return  __store(key, str, timestamp, isAck);
}

template <>
char *ArancinoClass::store(char *key, int value, char* timestamp, bool isAck)
{
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet =  __store(key, str, timestamp, isAck);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template <>
ArancinoPacket ArancinoClass::store<ArancinoPacket>(char *key, uint32_t value, bool isAck, char* timestamp)
{
	char str[20] = "";
	itoa(value, str, 10);
	return  __store(key, str, timestamp, isAck);
}

template <>
char *ArancinoClass::store(char *key, uint32_t value, char* timestamp, bool isAck)
{
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet =  __store(key, str, timestamp, isAck);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template <>
ArancinoPacket ArancinoClass::store<ArancinoPacket>(char *key, long value, bool isAck, char* timestamp)
{
	char str[20] = "";
	itoa(value, str, 10);
	return  __store(key, str, timestamp, isAck);
}

template <>
char *ArancinoClass::store(char *key, long value, char* timestamp, bool isAck)
{
	char str[20] = "";
	itoa(value, str, 10);
	ArancinoPacket packet = __store(key, str, timestamp, isAck);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

template <>
ArancinoPacket ArancinoClass::store<ArancinoPacket>(char *key, float value, char* timestamp, bool isAck)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	return  __store(key, str, timestamp, isAck);
}

template <>
char *ArancinoClass::store(char *key, float value, bool isAck, char* timestamp)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	ArancinoPacket packet =  __store(key, str, timestamp, isAck);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

ArancinoPacket ArancinoClass::__store( char* key, char* value, char* timestamp, bool isAck) {

	if(timestamp==NULL){
		 timestamp = getTimestamp();
	}

	if(key == NULL || value == NULL || strcmp(key, "") == 0){
		return invalidCommandErrorPacket;
	}

	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	JsonObject cmd_args = cmd_doc.createNestedObject("args");
	cmd_doc["cmd"] = STORE_COMMAND;
	JsonArray cmd_items = cmd_args.createNestedArray("items");
	JsonObject items_obj = cmd_items.createNestedObject();
	items_obj["key"] = key;
	items_obj["value"] = value;
	items_obj["ts"] = timestamp;

	JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
	cmd_cfg["ack"] = isAck? 0 : 1;
	return executeCommand(cmd_doc, ITEMS_RESPONSE);
}

/******** API BASIC :: MSTORE *********/

template <>
ArancinoPacket ArancinoClass::mstore<ArancinoPacket>(char** keys, char** values, int len, char* timestamp, bool isAck)
{
	if(timestamp==NULL){
		 timestamp = getTimestamp();
	}

	if ((keys == NULL) || (values == NULL) || (len <= 0))
	{
		return invalidCommandErrorPacket;
	}

	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	JsonObject cmd_args = cmd_doc.createNestedObject("args");
	cmd_doc["cmd"] = STORE_COMMAND;
	JsonArray cmd_items = cmd_args.createNestedArray("items");
	for (int i=0; i<len; i++){
		JsonObject items_obj = cmd_items.createNestedObject();
		items_obj["key"] = keys[i];
		items_obj["value"] = values[i];
		items_obj["ts"] = timestamp;
	}

	JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
	cmd_cfg["ack"] = isAck? 0 : 1;
	return executeCommand(cmd_doc, ITEMS_RESPONSE);
}

template<> 
void ArancinoClass::mstore(char** keys, char** values, int len, char* timestamp, bool isAck) {
	if(timestamp==NULL){
		 timestamp = getTimestamp();
	}
	if ((keys == NULL) || (values == NULL) || (len <= 0)) {
		return;
	}
	ArancinoPacket packet = mstore(keys, values, len, timestamp, isAck);
	free (packet);
}

template <>
char **ArancinoClass::mstore(char** keys, char** values, int len, char* timestamp, bool isAck)
{
	if(timestamp==NULL){
		 timestamp = getTimestamp();
	}
	if ((keys == NULL) || (values == NULL) || (len <= 0)) {
		return;
	}
	ArancinoPacket packet = mstore(keys, values, len, timestamp, isAck);
	if (!packet.isError)
		return packet.response.stringArray;
	else
		return NULL;
}

/******** API BASIC :: STORETAGS *********/

ArancinoPacket ArancinoClass::storetags(char *key, char **tags, char **values, int len, char* timestamp, bool isAck)
{
	if(timestamp==NULL){
		 timestamp = getTimestamp();
	}
	if ((key == NULL) || (tags == NULL) || (values == NULL) || (len <= 0))
		return invalidCommandErrorPacket;
	
	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	JsonObject cmd_args = cmd_doc.createNestedObject("args");
	cmd_doc["cmd"] = STORETAGS_COMMAND;
	JsonArray cmd_items = cmd_args.createNestedArray("items");
	for (int i=0; i<len; i++){
		JsonObject items_obj = cmd_items.createNestedObject();
		items_obj["tag"] = tags[i];
		items_obj["value"] = values[i];
	}
	cmd_args["ts"] = timestamp;

	JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
	cmd_cfg["ack"] = isAck? 0 : 1;
	return executeCommand(cmd_doc, VOID_RESPONSE);
}

/******** API BASIC :: SETRESERVED *********/

ArancinoPacket ArancinoClass::setReserved(char *key, char *value)
{
	if (key == NULL && value == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;
	return set<ArancinoPacket>(key, value, true, true, "rsvd");
}

/******** API BASIC :: GETRESERVED *********/

template <>
ArancinoPacket ArancinoClass::getReserved<ArancinoPacket>(char *key, bool id_prefix)
{
	if (key == NULL && strcmp(key, "") == 0)
		return invalidCommandErrorPacket;

	return get<ArancinoPacket>(key, true, "rsvd");
}

template <>
char *ArancinoClass::getReserved(char *key, bool id_prefix)
{
	ArancinoPacket packet = getReserved<ArancinoPacket>(key, id_prefix);
	if (!packet.isError)
		return packet.response.string;
	else
		return NULL;
}

char *ArancinoClass::getModuleVersion()
{
	char key[strlen(MODVERS_KEY) + 1];
	strcpy(key, MODVERS_KEY);
	char *retString = getReserved(key, false);
	return retString;
}

char *ArancinoClass::getModuleLogLevel()
{
	char key[strlen(MODLOGLVL_KEY) + 1];
	strcpy(key, MODLOGLVL_KEY);
	char *retString = getReserved(key, false);
	return retString;
}

char *ArancinoClass::getModuleEnvironment()
{
	char key[strlen(MODENV_KEY) + 1];
	strcpy(key, MODENV_KEY);
	char *retString = getReserved(key, false);
	return retString;
}

char *ArancinoClass::getBlinkId()
{
	char key[strlen(BLINK_ID_KEY) + 1];
	strcpy(key, BLINK_ID_KEY);
	char *retString = getReserved(key, true);
	return retString;
}

/******** API BASIC :: SETRESERVED *********/

ArancinoPacket ArancinoClass::setBlinkId(int value)
{
	char str[20] = "";
	itoa(value, str, 10);
	return setReserved(BLINK_ID_KEY, str, true);
}

/******** API UTILITY :: FREE *********/

void ArancinoClass::free(char *str)
{
#if defined(USEFREERTOS)
	vPortFree(str);
#else
	std::free(str);
#endif
}

void ArancinoClass::free(char **_array)
{
	char **dummy = (_array != NULL) ? _array - sizeof(char) : NULL;

	if (_array != NULL)
	{
#if defined(USEFREERTOS)
		vPortFree(*_array);
#else
		std::free(*_array);
#endif
	}
	if (dummy != NULL)
	{
#if defined(USEFREERTOS)
		vPortFree(dummy);
#else
		std::free(dummy);
#endif
	}
}

void ArancinoClass::free(ArancinoPacket packet)
{
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

void *ArancinoClass::malloc(size_t _size)
{
/* Call the FreeRTOS version of malloc. */
#if defined(USEFREERTOS)
	return pvPortMalloc(_size);
#else
	return std::malloc(_size);
#endif
}

void *ArancinoClass::calloc(size_t nmemb, size_t _size)
{
/* Call the FreeRTOS version of malloc. */
#if defined(USEFREERTOS)
#if defined(ARDUINO_ARANCINOV12_H743ZI) || defined(ARDUINO_ARANCINOV12_H743ZI2) || defined(ARDUINO_ARANCINO_VOLANTE) || defined(ARDUINO_ARCH_RP2040)
	uint8_t *ptr = (uint8_t *)malloc(nmemb * (_size));
	memset(ptr, 0, nmemb); // clear the buffer #pte4c0
	return ptr;
#else
	return pvPortCalloc(nmemb, _size);
#endif
#else
	return std::calloc(nmemb, _size);
#endif
}

/******** API UTILITY :: CHECK-UTF8 *********/

bool ArancinoClass::isValidUTF8(const char *string) // From: https://stackoverflow.com/a/28311607
{
	if (!string)
		return true;

	const unsigned char *bytes = (const unsigned char *)string;
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

void ArancinoClass::print(int value)
{
	char str[20] = "";
	itoa(value, str, 10);
	print(str);
}

void ArancinoClass::print(float value)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	print(str);
}

void ArancinoClass::print(double value)
{
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	print(str);
}

/******** API ADVANCED :: PRINTLN *********/

void ArancinoClass::println(String value)
{
	print(value + String('\n'));
}

void ArancinoClass::println(int value)
{
	print(String(value) + String('\n'));
}

void ArancinoClass::println(float value)
{
	char str[20] = "";
	_floatToString(value, decimal_digits, str);
	print(str + String('\n'));
}

void ArancinoClass::println(double value)
{
	char str[20] = "";
	_doubleToString(value, decimal_digits, str);
	print(str + String('\n'));
}

int ArancinoClass::getArraySize(char **_array)
{
	char **dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
	return dummy != NULL ? (int)(*dummy) : 0;
}

int ArancinoClass::getArraySize(String *_array)
{
	String *dummy = (_array != NULL) ? _array - 1 : NULL;
	return (dummy[0] != "") ? dummy[0].toInt() : 0;
}

char *ArancinoClass::getTimestamp()
{

	// unsigned long ts = rtc.getEpoch(); //10 digits
	unsigned long millis_current = millis();
	unsigned long mls_tmp = millis_current - millis_previous;
	millis_previous = millis_current;
	if (mls_tmp < 0)
	{ // overflow of millis
		mls_tmp = MAX_UNSIGNED_LONG_VALUE - millis_previous + millis_current;
	}
	tmst_inf = tmst_inf + mls_tmp;
	if (tmst_inf > 999999999)
	{
		tmst_sup = tmst_sup + 1;
		tmst_inf = tmst_inf % 1000000000;
	}
	char ts_sup_tmp[5];
	sprintf(ts_sup_tmp, "%04ld", tmst_sup); // 4 digits
	memcpy(timestamp, ts_sup_tmp, 4);
	char ts_inf_tmp[10];
	sprintf(ts_inf_tmp, "%09ld", tmst_inf); // 9 digits
	memcpy(&timestamp[4], ts_inf_tmp, 9);
	return timestamp;
}

/******** INTERNAL UTILS :: FREE *********/

ArancinoPacket ArancinoClass::executeCommand(char* cmd, char* key, char* field, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type){
	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	StaticJsonDocument<RSP_DOC_SIZE> rsp_doc;
	_buildArancinoJson(cmd_doc, cmd, key, field, argsHasItems, itemsHasDict, cfg);

	#if defined(USEFREERTOS)
	if (takeCommMutex((TickType_t)portMAX_DELAY) != pdFALSE) {
	#endif
		_iface->_sendArancinoCommand(cmd_doc);
		if(cmd_doc["cfg"]["ack"] == 1)
			_iface->_receiveArancinoResponse(rsp_doc);
	#if defined(USEFREERTOS)
		giveCommMutex();
	}
	#endif

	if(rsp_doc.isNull()) 
		return communicationErrorPacket;
 
	ArancinoPacket packet = createArancinoPacket(rsp_doc, response_type);
	return packet;
}

ArancinoPacket executeCommand(char* cmd, char** keys, char** fields, int len, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg, int response_type){
	StaticJsonDocument<CMD_DOC_SIZE> cmd_doc;
	StaticJsonDocument<RSP_DOC_SIZE> rsp_doc;
	_buildArancinoJson(cmd_doc, cmd, keys, fields, len, argsHasItems, itemsHasDict, cfg);
	
	#if defined(USEFREERTOS)
	if (takeCommMutex((TickType_t)portMAX_DELAY) != pdFALSE) {
	#endif
		_iface->_sendArancinoCommand(cmd_doc);
		if(cmd_doc["cfg"]["ack"] == 1)
			_iface->_receiveArancinoResponse(rsp_doc);
	#if defined(USEFREERTOS)
		giveCommMutex();
	}
	#endif

	if(rsp_doc.isNull()) 
		return communicationErrorPacket;
 
	ArancinoPacket packet = createArancinoPacket(rsp_doc, response_type);
	return packet;
}

ArancinoPacket executeCommand(JsonDocument& cmd_doc, int response_type){
	StaticJsonDocument<RSP_DOC_SIZE> rsp_doc;

	#if defined(USEFREERTOS)
	if (takeCommMutex((TickType_t)portMAX_DELAY) != pdFALSE) {
	#endif
		_iface->_sendArancinoCommand(cmd_doc);
		if(cmd_doc["cfg"]["ack"] == 1)
			_iface->_receiveArancinoResponse(rsp_doc);
	#if defined(USEFREERTOS)
		giveCommMutex();
	}
	#endif

	if(rsp_doc.isNull()) 
		return communicationErrorPacket;
 
	ArancinoPacket packet = createArancinoPacket(rsp_doc, response_type);
	return packet;
}

ArancinoPacket ArancinoClass::createArancinoPacket(JsonDocument& response_dict, int response_type){
	ArancinoPacket packet;

	switch(response_type){
		case VOID_RESPONSE: {
			ArancinoPacket temp = {false, response_dict["rsp_code"], VOID, {.string = NULL}};
			packet = temp;
			break;
		}
		case KEY_VALUE_RESPONSE: {
			JsonArray resp_items = response_dict["args"]["items"];
			int resp_size = resp_items.size();
			if (resp_size > 0 && resp_size < 2) {
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING, {.string = resp_items[0]["value"]}};
				packet = temp;
			} else if (resp_size > 1) {
				const char* resp_values[resp_size];
				for (int i=0; i<resp_size; i++){
					resp_values[i] = resp_items[i]["value"];
				}
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING_ARRAY, {.stringArray = resp_values}};
				packet = temp;
			}
			break;
		}
		case KEY_RESPONSE: {
			ArancinoPacket temp = {false, response_dict["rsp_code"], INT, {.integer = response_dict["args"]["keys"]}};
			packet = temp;
			break;
		}
		case FIELDS_ITEMS_RESPONSE: {
			JsonArray resp_items = response_dict["args"]["items"];
			int resp_size = resp_items.size();
			if (resp_size > 0 && resp_size < 2) {
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING, {.string = response_dict["args"]["items"][0]["value"]}};
				packet = temp;
			} else if (resp_size > 1) {
				const char* resp_values[resp_size];
				for (int i=0; i<resp_size; i++){
					resp_values[i] = response_dict["args"]["items"][0]["value"];
				}
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING_ARRAY, {.stringArray = resp_values}};
				packet = temp;
			}
			break;
		}
		case ITEMS_RESPONSE: {
			JsonArray resp_items = response_dict["args"]["items"];
			if (resp_size > 0 && resp_size < 2) {
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING, {.string = resp_items[0]}};
				packet = temp;
			} else if (resp_size > 1) {
				const char* resp_values[resp_size];
				for (int i=0; i<resp_size; i++){
					resp_values[i] = resp_items[i];
				}
				ArancinoPacket temp = {false, response_dict["rsp_code"], STRING_ARRAY, {.stringArray = resp_values}};
				packet = temp;
			}
			break;
		}
		case CLIENTS_RESPONSE: {
			ArancinoPacket temp = {false, response_dict["rsp_code"], INT, {.integer = response_dict["args"]["clients"]}};
			packet = temp;
			break;
		}
		default:
			return invalidCommandErrorPacket;
	}
	
}

void ArancinoClass::_buildArancinoJson(JsonDocument& cmd_doc, char* cmd, char* key, char* field, char* value, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg){
  JsonObject cmd_args = cmd_doc.createNestedObject("args");
  cmd_doc["cmd"] = cmd;

  if (argsHasItems){
    JsonArray cmd_items = cmd_args.createNestedArray("items");
    if (itemsHasDict){
      JsonObject items_obj = cmd_items.createNestedObject();
      
      if (key){
        items_obj["key"] = key;
      }

      if (field){
        items_obj["field"] = field;
      }

	  if(value){
		items_obj["value"] = value;
	  }
    } else {
      cmd_items.add(key);
    }
  }

  JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
  if (cfg.pers){
    cmd_cfg["pers"] = cfg.pers == CFG_FALSE ? 0 : 1;
  }
  if (cfg.ack){
    cmd_cfg["ack"] = cfg.ack == CFG_FALSE ? 0 : 1;
  }
  if (cfg.type){
    cmd_cfg["type"] = cfg.type;
  }
}

void ArancinoClass::_buildArancinoJson(JsonDocument& cmd_doc, char* cmd, char** keys, char** fields, char** values, int len, bool argsHasItems, bool itemsHasDict, ArancinoCFG cfg){
	JsonObject cmd_args = cmd_doc.createNestedObject("args");
	cmd_doc["cmd"] = cmd;

	if (argsHasItems){
		JsonArray cmd_items = cmd_args.createNestedArray("items");
		if (itemsHasDict){
			for (int i=0; i<len; i++) {
				JsonObject items_obj = cmd_items.createNestedObject();
				if (keys){
					items_obj["key"] = keys[i];
				}
				if (fields){
					items_obj["field"] = fields[i];
				}
				if (values){
					items_obj["value"] = values[i];
				}
			}
		} else {
			for (int i=0; i<len; i++) {
				if(keys){
					cmd_items.add(keys[i]);
				}
			}
		}
	}

	JsonObject cmd_cfg = cmd_doc.createNestedObject("cfg");
	if (cfg.pers){
		cmd_cfg["pers"] = cfg.pers == CFG_FALSE ? 0 : 1;
	}
	if (cfg.ack){
		cmd_cfg["ack"] = cfg.ack == CFG_FALSE ? 0 : 1;
	}
	if (cfg.type){
		cmd_cfg["type"] = cfg.type;
	}
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

void ArancinoClass::_doubleToString(double value, unsigned int _nDecimal, char *str)
{
	noInterrupts();
	char val[20] = "";
	sprintf(val, "%d", int(value));
	int valueLength = strlen(val);
	if (valueLength + _nDecimal > 15)
	{ // The double data type has 15 decimal digits of precision
		_nDecimal = 15 - valueLength;
		if (_nDecimal < 0)
		{
			_nDecimal = 0;
		}
	}
	double rounding = 0.5;
	rounding /= pow(10, _nDecimal);
	int decimalPart = pow(10, _nDecimal) * (value - int(value) + rounding);
	char dec[2];
	itoa(_nDecimal, dec, 10);
	char param[10];
	strcpy(param, "%d.%0");
	strcat(param, dec);
	strcat(param, "d");
	sprintf(str, param, int(value), decimalPart);
	interrupts();
	// return str;
	// dtostrf(value, valueLength, _nDecimal, str);
}

void ArancinoClass::_floatToString(float value, unsigned int _nDecimal, char *str)
{
	noInterrupts();
	dtostrf(value,7,_nDecimal,str);
	interrupts();	
}

int ArancinoClass::_getDigit(long value)
{
	int digit = 0;
	long _val = value;
	while (abs(_val) > 0)
	{
		++digit;
		_val /= 10;
	}
	return digit;
}

void ArancinoClass::_printDebugMessage(char* value) {
	if (_isDebug && _commMode){
	__publish(MONITOR_KEY, value);
	__set(MONITOR_KEY, value, false);
	} else if(_isDebug && _dbgSerial != NULL){
		_dbgSerial->print(value);
	}
}

int ArancinoClass::_getResponseCode(char *message)
{
	int value = -1;
	int separatorIndex = -1;
	char *temp = NULL;
	char *charAddr = strchr(message, DATA_SPLIT_CHAR); // address of DATA_SPLIT_CHAR
	if (charAddr == NULL)							   // NO DATA_SPLIT_CHAR
	{
		charAddr = strchr(message, END_TX_CHAR); //-> search for END_TX_CHAR
	}

	if (charAddr != NULL)
	{
		separatorIndex = charAddr - message; // index of DATA_SPLIT_CHAR/END_TX_CHAR on message string
		temp = (char *)calloc(separatorIndex + 1, sizeof(char));
		strncpy(temp, message, separatorIndex + 1);
		temp[separatorIndex] = '\0'; // replace separator with null-character

		value = atoi(temp);
		free(temp);
	}
	else
	{
		value = -1; // error
	}

	return value;
}

char *ArancinoClass::_parse(char *message)
{

	char *status = NULL;
	char *value = NULL;
	int DSCIndex;									   // DATA_SPLIT_CHAR index
	char *charAddr = strchr(message, DATA_SPLIT_CHAR); // address of DATA_SPLIT_CHAR

	if (charAddr != NULL)
	{
		DSCIndex = charAddr - message; // index of DATA_SPLIT_CHAR on message string
	}
	else
	{
		DSCIndex = -1; // DATA_SPLIT_CHAR not found --> received only 'status'
	}

	int messageLength = strlen(message);

	if (DSCIndex == -1)
	{
		status = (char *)calloc(messageLength + 1, sizeof(char)); // response = [STATUS] + @
		strncpy(status, message, messageLength);
		status[messageLength] = '\0'; // replace END_TX_CHAR with '\0'
	}
	else
	{
		status = (char *)calloc(DSCIndex + 1, sizeof(char)); // response = [STATUS] + # + [VALUE] + @
		strncpy(status, message, DSCIndex);
		status[DSCIndex] = '\0'; // replace DATA_SPLIT_CHAR with '\0'

		value = (char *)calloc(messageLength - DSCIndex - 1, sizeof(char));
		strncpy(value, &message[DSCIndex + 1], messageLength - (DSCIndex + 2));
		value[messageLength - (DSCIndex + 2)] = '\0'; // replace END_TX_CHAR with '\0'
	}

	free(status);

	if (strcmp(value, nullStr) == 0)
	{
		free(value);
		return NULL;
	}
	else
	{
		return value;
	}
}

char **ArancinoClass::_parseArray(char *data)
{
	char **arrayParsed = NULL;
	char *tempArray;

	char *previousDSCIndex = data;
	char *DSCIndex = data;

	int fieldCount = data != NULL; // at least 1 field
	int maxLength = 0;

	if (data != NULL)
	{

		// get the key count
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
				maxLength = strlen(previousDSCIndex);
			}
			previousDSCIndex = DSCIndex + 1;

		} while (DSCIndex != NULL);

		/*
		 * Alloco un elemento in più (fieldCount + 1).
		 * Nel primo elemento inserisco la lunghezza del vettore;
		 * Il valore di ritorno della funzione sarà l'elemento 1 del vettore (e non l'elemento 0)
		 */
		arrayParsed = (char **)malloc((fieldCount + 1) * sizeof(char *));			   // user must free!
		tempArray = (char *)malloc((fieldCount + 1) * (maxLength + 1) * sizeof(char)); // user must free!
		arrayParsed[0] = (char *)fieldCount;

		previousDSCIndex = data;

		for (int i = 1; i < (fieldCount + 1); ++i)
		{
			arrayParsed[i] = tempArray + ((i - 1) * (maxLength + 1));
			DSCIndex = strchr(previousDSCIndex + 1, DATA_SPLIT_CHAR);

			if (DSCIndex != NULL)
			{
				if (strncmp(previousDSCIndex, nullStr, DSCIndex - previousDSCIndex) == 0)
				{
					arrayParsed[i] = NULL;
				}
				else
				{
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

		if (data != NULL)
		{
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

void ArancinoClass::taskSuspend()
{
#if defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		vTaskSuspendAll();
	}
#endif
}

void ArancinoClass::taskResume()
{
#if defined(USEFREERTOS)
	if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED)
	{
		xTaskResumeAll();
	}
#endif
}

ArancinoClass Arancino;
