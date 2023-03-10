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

#ifndef __ARANCINODEFINITIONS_H__
#define __ARANCINODEFINITIONS_H__
#include <cstdint>

#ifndef MCU_FAMILY
#define MCU_FAMILY (const char*)"undefined"
#endif

#ifdef BAUDRATE
#undef BAUDRATE
#endif

#ifdef TIMEOUT
#undef TIMEOUT
#endif

#ifndef ARANCINO_CORE_VERSION
#define ARANCINO_CORE_VERSION "0.0.0"
#endif

#if defined(__SAMD21G18A__)
const uint32_t BAUDRATE = 4000000;
const uint32_t TIMEOUT = 1000;
#define SERIAL_PORT SerialUSB
#define SERIAL_DEBUG Serial
#elif defined(ARDUINO_ARCH_RP2040) && defined(USEFREERTOS)
const uint32_t BAUDRATE = 256000;
const uint32_t TIMEOUT = 10000;
#if defined(USE_TINYUSB)
#define SERIAL_PORT SerialTinyUSB
#else
#define SERIAL_PORT Serial
#endif
#define SERIAL_DEBUG Serial1
#elif defined(ARDUINO_ARCH_ESP32)
/*
    Enable CUSTOM_SERIAL if you want to use a custom serial port
    other than Serial1 (SERIAL_PORT defined below)
*/
#define CUSTOM_SERIAL

const uint32_t BAUDRATE = 115200;
const uint32_t TIMEOUT = 10000;

#if defined(CUSTOM_SERIAL)
#define SERIAL_PORT SerialPort
#define UARTx   1 //UART1
#define UART_TX   13
#define UART_RX   15
#else
/*
    If CUSTOM_SERIAL is disabled and you want to use a standard 
    serial (e.g. Serial 1) define it here:
*/
#define SERIAL_PORT Serial1
#endif
#define SERIAL_DEBUG Serial
#elif defined(ARDUINO_ARCH_NRF52)
const uint32_t BAUDRATE = 256000;
const uint32_t TIMEOUT = 10000;
#define SERIAL_PORT Serial
#define SERIAL_DEBUG Serial1
#else
const uint32_t BAUDRATE = 256000;
const uint32_t TIMEOUT = 10000;
#define SERIAL_PORT Serial
#define SERIAL_DEBUG Serial1
#endif

#define SERIAL_TRANSPORT typeof(SERIAL_PORT)
const uint32_t BAUDRATE_DEBUG = 115200;

#define START_COMMAND			(const char*)"START"
#define MSET_COMMAND			(const char*)"MSET"
#define MSET_PERS_COMMAND		(const char*)"MSETPERS"
#define SET_COMMAND 			(const char*)"SET"
#define SET_PERS_COMMAND 		(const char*)"SETPERS"
#define MGET_COMMAND			(const char*)"MGET"
#define GET_COMMAND 			(const char*)"GET"
#define DEL_COMMAND 			(const char*)"DEL"
#define KEYS_COMMAND			(const char*)"KEYS"
#define HGET_COMMAND			(const char*)"HGET"
#define HGETALL_COMMAND			(const char*)"HGETALL"
#define HKEYS_COMMAND			(const char*)"HKEYS"
#define HDEL_COMMAND			(const char*)"HDEL"
#define HSET_COMMAND			(const char*)"HSET"
#define HSET_PERS_COMMAND		(const char*)"HSETPERS"
#define HVALS_COMMAND			(const char*)"HVALS"
#define PUBLISH_COMMAND			(const char*)"PUB"
#define FLUSH_COMMAND			(const char*)"FLUSH"
#define STORE_COMMAND			(const char*)"STORE"
#define MSTORE_COMMAND			(const char*)"MSTORE"
#define STORETAGS_COMMAND		(const char*)"STORETAGS"
#define GETRESERVED_COMMAND		(const char*)"GETRSVD"
#define SETRESERVED_COMMAND		(const char*)"SETRSVD"

#define SENT_STRING				"Sent Command: "
#define RCV_STRING				"Received Response: "

#define END_TX_CHAR				(const char)4 //'@' //
#define DATA_SPLIT_CHAR			(const char)30 //'#' //
#define ARRAY_SPLIT_CHAR		(const char)16 //'%' //
#define NULL_CHAR				(const char)25
#define ID_SEPARATOR			"_"

#define RSP_OK							100
#define RSP_HSET_NEW					101
#define RSP_HSET_UPD					102
#define ERR_GENERIC						200		//Generic Error
#define ERR_NULL						201		//Null value
#define ERR_SET							202		//Error during SET
#define ERR_CMD_NOT_FND					203		//Command Not Found
#define ERR_CMD_NOT_RCV					204		//Command Not Received
#define ERR_CMD_PRM_NUM					205		//Invalid parameter number
#define ERR_REDIS						206		//Generic Redis Error
#define ERR_REDIS_KEY_EXISTS_IN_STD	    207		//Key exists in the Standard Data Store
#define ERR_REDIS_KEY_EXISTS_IN_PERS	208		//Key exists in the Persistent Data Store
#define ERR_NON_COMPATIBILITY		    209		//Non compatibility between Arancino Module and Library
#define ERR_INVALID_ARGUMENTS           210     //Generic Invalid Arguments
#define ERR_VALUE           		    211		//Invalid value
#define ERR_CMD_TYPE                    212     //Error during packet transmission

#define DBG_PIN					26		//pin used to Debug Message
//#define PWR_PIN				??		//pin used for Power Management

#define MONITOR_KEY				(const char*)"___MONITOR___"
#define POWER_KEY				(const char*)"___POWER___"
#define MODVERS_KEY				(const char*)"___MODVERS___"
#define MODLOGLVL_KEY			(const char*)"___MODLOGLVL___"
#define MODENV_KEY              (const char*)"___MODENV___"
#define BLINK_ID_KEY			(const char*)"___BLINK_ID___"
#define LIB_VERSION				(const char*)"2.0.0"	//library version
#define CRTX_VERSION            (const char*)"1.0.0"  //cortex version

//RESPONSE CODE
#define INVALID_VALUE_ERROR     -4
#define RESERVED_KEY_ERROR		-3
#define COMMUNICATION_ERROR		-2
#define GENERIC_ERROR			-1
#define VOID_ERROR_TYPE          0

//RESPONSE TYPE
#define VOID			(int)0
#define INT				(int)1
#define STRING			(int)2
#define STRING_ARRAY	(int)3

// //RESERVED KEYS ARRAY DEF
// #define RESERVED_KEY_ARRAY_SIZE		4   // Array dimension of Reserved Keys
// #define RESERVED_KEY_MAX_LENGTH		12  // Max length of a Reserved Key name

//MAX UNSIGNED LONG VALUE
#define MAX_UNSIGNED_LONG_VALUE     4294967295

#if defined(ARDUINO_ARCH_ESP32)
//ESP32 configMAX_PRIORITIES is 25
#define ARANCINO_TASK_PRIORITY      (ESP_TASK_TCPIP_PRIO - 1)
#define SERVICETASK_STACK           (1024 * 4)
#elif defined(__SAMD21G18A__)
#define ARANCINO_TASK_PRIORITY      configMAX_PRIORITIES - 1
#define SERVICETASK_STACK           768
#elif defined(ARDUINO_ARCH_RP2040)
#define ARANCINO_TASK_PRIORITY      configMAX_PRIORITIES - 1
#define SERVICETASK_STACK           1024
#elif defined(ARDUINO_ARCH_NRF52)
#define ARANCINO_TASK_PRIORITY      TASK_PRIO_HIGH - 1
#define SERVICETASK_STACK           2048
#else
#warning "This platform has not been tested!"
#define ARANCINO_TASK_PRIORITY      configMAX_PRIORITIES - 1
#define SERVICETASK_STACK           1024
#endif

#define HEARTBEAT_PERIOD            (pdMS_TO_TICKS(10000))
#define IDENTIFICATION_PERIOD       (pdMS_TO_TICKS(60000))
#define INTEROCEPTION_PERIOD        (pdMS_TO_TICKS(10000))
#define SERVICE_TASK_PERIOD         (pdMS_TO_TICKS(5000))

#define SERIAL_MAX_RETRIES          10
#define MQTT_MAX_RETRIES            500
#define BLUETOOTH_MAX_RETRIES       100

#define ID_SIZE IDSIZE*2 //IDSIZE will be provided by microcontroller-id library

//MsgPack
#define CMD_DOC_SIZE 512
#define RSP_DOC_SIZE 256
#define CFG_UNSET 0
#define CFG_TRUE 1
#define CFG_FALSE 2

#define VOID_RESPONSE 0
#define KEY_VALUE_RESPONSE 1
#define KEYS_RESPONSE 2
#define FIELDS_ITEMS_RESPONSE 3
#define ITEMS_RESPONSE 4
#define CLIENTS_RESPONSE 5

#endif