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


#if defined(__SAMD21G18A__)
#define BAUDRATE 4000000
#define TIMEOUT 100
#define SERIAL_PORT SerialUSB
#else
#define BAUDRATE 256000
#define TIMEOUT 100
#define SERIAL_PORT Serial
#endif

#define SERIAL_TRANSPORT typeof(SERIAL_PORT)

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
#define LIB_VERSION				"0.1.2"	//library version 

//RESPONSE TYPE
#define ERROR           -1
#define VOID            0
#define INT             1
#define STRING          2
#define STRING_ARRAY    3
