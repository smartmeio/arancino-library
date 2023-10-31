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

#ifndef MCU_FAMILY
#define MCU_FAMILY (char*)"undefined"
#endif

#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define TIMEOUT 100
#define SERIAL_PORT SerialUSB
#define SERIAL_DEBUG Serial
#elif defined(ARDUINO_ARCH_RP2040) && defined(USEFREERTOS)
#define BAUDRATE 256000
#define TIMEOUT 10000
#if defined(USE_TINYUSB)
#define SERIAL_PORT SerialTinyUSB
#else
#define SERIAL_PORT Serial
#endif
#define SERIAL_DEBUG Serial1
#else
#define BAUDRATE 256000
#define TIMEOUT 10000
#define SERIAL_PORT Serial
#define SERIAL_DEBUG Serial1
#endif

#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

#define START_COMMAND			(char*)"START"
#define MSET_COMMAND			(char*)"MSET"
#define MSET_PERS_COMMAND		(char*)"MSETPERS"
#define SET_COMMAND 			(char*)"SET"
#define SET_PERS_COMMAND 		(char*)"SETPERS"
#define MGET_COMMAND			(char*)"MGET"
#define GET_COMMAND 			(char*)"GET"
#define DEL_COMMAND 			(char*)"DEL"
#define KEYS_COMMAND			(char*)"KEYS"
#define HGET_COMMAND			(char*)"HGET"
#define HGETALL_COMMAND			(char*)"HGETALL"
#define HKEYS_COMMAND			(char*)"HKEYS"
#define HDEL_COMMAND			(char*)"HDEL"
#define HSET_COMMAND			(char*)"HSET"
#define HSET_PERS_COMMAND		(char*)"HSETPERS"
#define HVALS_COMMAND			(char*)"HVALS"
#define PUBLISH_COMMAND			(char*)"PUB"
#define FLUSH_COMMAND			(char*)"FLUSH"
#define STORE_COMMAND			(char*)"STORE"
#define MSTORE_COMMAND			(char*)"MSTORE"
#define STORETAGS_COMMAND		(char*)"STORETAGS"
#define GETRESERVED_COMMAND		(char*)"GETRSVD"
#define SETRESERVED_COMMAND		(char*)"SETRSVD"

#define SENT_STRING				"Sent Command: "
#define RCV_STRING				"Received Response: "

#define END_TX_CHAR				(char)4 //'@' //
#define DATA_SPLIT_CHAR			(char)30 //'#' //
#define ARRAY_SPLIT_CHAR		(char)16 //'%' //
#define NULL_CHAR				(char)25
#define ID_SEPARATOR			"_"

#define RSP_OK							100
#define RSP_HSET_NEW					101
#define RSP_HSET_UPD					102
#define ERR								200		//Generic Error
#define ERR_NULL						201		//Null value
#define ERR_SET							202		//Error during SET
#define ERR_CMD_NOT_FND					203		//Command Not Found
#define ERR_CMD_NOT_RCV					204		//Command Not Received
#define ERR_CMD_PRM_NUM					205		//Invalid parameter number
#define ERR_REDIS						206		//Generic Redis Error
#define ERR_REDIS_KEY_EXISTS_IN_STD	    207		//Key exists in the Standard Data Store
#define ERR_REDIS_KEY_EXISTS_IN_PERS	208		//Key exists in the Persistent Data Store
#define ERR_NON_COMPATIBILITY		    209		//Non compatibility between Arancino Module and Library
#define ERR_STORE_CONVERSION		    211		//Non compatibility between Arancino Module and Library

#define DBG_PIN					26		//pin used to Debug Message
//#define PWR_PIN				??		//pin used for Power Management

#define MONITOR_KEY				(char*)"___MONITOR___"
#define POWER_KEY				(char*)"___POWER___"
#define MODVERS_KEY				(char*)"___MODVERS___"
#define MODLOGLVL_KEY			(char*)"___MODLOGLVL___"
#define MODENV_KEY              (char*)"___MODENV___"
#define BLINK_ID_KEY			(char*)"___BLINK_ID___"
#define LIB_VERSION				(char*)"2.1.2"	//library version

//RESPONSE CODE
#define INVALID_VALUE_ERROR     -4
#define RESERVED_KEY_ERROR		-3
#define COMMUNICATION_ERROR		-2
#define GENERIC_ERROR			-1

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

#define ARANCINO_TASK_PRIORITY      2
