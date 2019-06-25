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

//TODO: replace multiple priority queue with a single priority queue

#include "Arancino.h"
//#include "ThreadSafeUART.h"

#define DEBUG 0
#define MAX_MSG_LENGTH 1024 //in char - not used
#define USE_PRIORITIES 0

/*
 * Element of the queue that contain the requests to be sent through the UART.
 * One struct contain only one Arancino request and the task id from which it comes.
 */
typedef struct _msgItem
{
    String msg;
    uint32_t taskID;
    TaskHandle_t taskHandle = NULL;
    _msgItem *next = NULL;
} msgItem;

/*
 * Element that defines the Arancino requests queue.
 * The struct contains the head of the queue and the count of the requests ready to be sent
 */
typedef struct
{
    uint32_t msgCount;
    _msgItem *head;
} msgQueue;

void initQueue(msgQueue *);

int insertRequest(uint16_t, uint32_t, String);
msgItem getRequest(uint16_t);
int getRqstCount(uint16_t);

int insertResponse(uint16_t, uint32_t, String);
String getResponse(uint16_t, uint16_t);
int getResponseCount(uint16_t, uint16_t);

void printQueue(msgQueue *);


/*
 * Vector of queues that will contain the requests to be sent via uart.
 * The length of the vector is defined by the number of priorities allowed for FreeRTOS tasks.
 */
#if defined(USE_PRIORITIES) && USE_PRIORITIES == 1
msgQueue rqstByPriority[configMAX_PRIORITIES];
#else
msgQueue rqstByPriority[1];
#endif

/*
 * Vector of queues that will contain the responses received via uart.
 * The length of the vector is defined by the number of priorities allowed for FreeRTOS tasks.
 */
#if defined(USE_PRIORITIES) && USE_PRIORITIES == 1
msgQueue responseByPriority[configMAX_PRIORITIES];
#else
msgQueue responseByPriority[1];
#endif

/*
 * Once the scheduler is launched, this should be the only task that can write to the uart (stream).
 * The reading/writing on the uart is regulated by the uartMutex.
 * Between one request and another there is a 50 millisecond delay, during which the task is suspended.
 * The serial port used is passed as parameter during the task initialization (on begin).
 */
void commTask( void *pvParameters )
{
	Stream *stream = (Stream *)pvParameters;
	
    for (int i = 0; i < configMAX_PRIORITIES; i++)
    {
        initQueue(&rqstByPriority[i]);
        initQueue(&responseByPriority[i]);        
    }
    
    int temp = -1;
    String start;
	
    while(1)
    {
        //Priorities system currently not implemented
		if (getRqstCount(USE_PRIORITIES) > 0)
		{
            msgItem rqst = getRequest(USE_PRIORITIES);
            (*stream).print(rqst.msg);
            //BUG: sembra che le stringhe inviate un carattere per volta vengono troncate (e quindi viene ricevuto un comando parziale)
            /*for (int i = 0; i < (rqst.msg).length(); i++)
            {
                (*stream).print((rqst.msg)[i]);
            }*/
            String response = (*stream).readStringUntil(END_TX_CHAR);
            insertResponse(USE_PRIORITIES, rqst.taskID, response);
            vTaskResume(rqst.taskHandle); //waking up the task that made the request
		}
#if defined(DEBUG) && DEBUG == 1
		Serial.print("freeHeap: ");
        Serial.println( xPortGetFreeHeapSize() );
#endif
		vTaskDelay( 25 / portTICK_PERIOD_MS );	

    }
}

void initQueue(msgQueue *queue)
{
    queue->msgCount = 0;
    queue->head = NULL;
}

/*
 * Function that insert a string/character in the uart write queue.
 * The parameters are:
 *  priority of the calling task;
 *  ID of the calling task;
 *  msg that will be insert in queue;
 * 
 * The priority indicates the position on the queues vector.
 * taskID is used to group the strings that come from the same task. 
 * For request with the same task id, once the END_TX_CHAR character is received, 
 *  all subsequent characters are inserted into a new element.
 * 
 */
int insertRequest(uint16_t priority, uint32_t taskID, String msg)
{
    int toBeSuspended = 0;
    
    vTaskSuspendAll();
    msgItem *selectedMsg = NULL;
    
    if (rqstByPriority[priority].head == NULL)
    {
        rqstByPriority[priority].head = new msgItem;
        if (rqstByPriority[priority].head != NULL) //successfully allocated
        {
            (rqstByPriority[priority].head)->taskID = taskID;
            (rqstByPriority[priority].head)->next = NULL;
            selectedMsg = (rqstByPriority[priority].head);
        }
        else
        {
            return -1; //error
        }
    }
    else
    {
        msgItem *prevMsg = NULL;
        msgItem *currMsg = rqstByPriority[priority].head;
        
        while (currMsg != NULL && (currMsg->taskID != taskID || (currMsg->msg)[(currMsg->msg).length() - 1] == END_TX_CHAR))
        {
            prevMsg = currMsg;
            currMsg = currMsg->next;
        }
        
        if (currMsg == NULL) //coda terminata
        {
            msgItem *temp = new msgItem;
            temp->taskID = taskID;
            temp->next = NULL;
            prevMsg->next = temp;
            selectedMsg = prevMsg->next;
        }
        else
        {
            selectedMsg = currMsg;
        }
    }
    
    if (selectedMsg != NULL && msg != "")
    {
        int index = msg.indexOf(END_TX_CHAR); //-1 = carattere non trovato
            
        if (index > -1)
        {
            toBeSuspended = 1;
            //flag the task as toBeSuspended
            selectedMsg->taskHandle = xTaskGetCurrentTaskHandle();;
            selectedMsg->msg += msg.substring(0, index + 1);
            ++(rqstByPriority[priority].msgCount);
            insertRequest(priority, taskID, msg.substring(index + 1));
        }
        else
        {
            selectedMsg->msg += msg;
        }
    }

    xTaskResumeAll();
    //task must be suspended here
    if (toBeSuspended)
    {
        vTaskSuspend(NULL);  // Suspend ourselves.
    }
    
return 0;
}


/*
 * Function that insert a string/character in the uart write queue.
 * The parameters are:
 *  priority of the calling task;
 *  ID of the calling task;
 *  msg that will be insert in queue;
 * 
 * The priority indicates the position on the queues vector.
 * taskID is used to group the strings that come from the same task. 
 * For request with the same task id, once the END_TX_CHAR character is received, 
 *  all subsequent characters are inserted into a new element.
 * 
 */
int insertResponse(uint16_t priority, uint32_t taskID, String msg)
{
    vTaskSuspendAll();
	{
		msgItem *selectedMsg = NULL;
		
		if (responseByPriority[priority].head == NULL)
		{
			responseByPriority[priority].head = new msgItem;
			if (responseByPriority[priority].head != NULL) //successfully allocated
			{
				(responseByPriority[priority].head)->taskID = taskID;
				(responseByPriority[priority].head)->next = NULL;
				selectedMsg = (responseByPriority[priority].head);
			}
			else
			{
				return -1; //error
			}
		}
		else
		{
			msgItem *currMsg = responseByPriority[priority].head;

            while(currMsg->next != NULL)
            {
                currMsg = currMsg->next;
            }
			
			if (currMsg->next == NULL) //coda terminata
			{
				msgItem *temp = new msgItem;
				temp->taskID = taskID;
				temp->next = NULL;
				currMsg->next = temp;
				selectedMsg = currMsg->next;
			}
			else
			{
				selectedMsg = currMsg;
			}
		}
		
		if (selectedMsg != NULL && msg != "")
		{
            selectedMsg->msg = msg;
            ++(responseByPriority[priority].msgCount);
		}
	}
    xTaskResumeAll();

return 0;
}


/*
 * return the request count of the selected queue.
 */
int getRqstCount(uint16_t priority)
{
	int retVal = -1;
    vTaskSuspendAll();
	{
		retVal = rqstByPriority[priority].msgCount;
	}
    xTaskResumeAll();
	return retVal;
}

/*
 * return the request count of the selected queue.
 */
int getResponseCount(uint16_t priority, uint16_t taskID)
{
	int retVal = 0;
    vTaskSuspendAll();
	//if( xSemaphoreTake( responseQueueMutex, ( TickType_t ) MSG_MUTEX_TIMEOUT ) == pdTRUE )
	{
        msgItem *response = responseByPriority[priority].head;
        while (response != NULL)
        {
            if (response->taskID == taskID)
                ++retVal;
            response = response->next;
        }
		//xSemaphoreGive(responseQueueMutex);
	}
	xTaskResumeAll();
	return retVal;
}


/*
 * Function that return the string of the oldest request for the selected priority.
 */
msgItem getRequest(uint16_t priority)
{
    msgItem retMsgItem;
    vTaskSuspendAll();
	{
		if (rqstByPriority[priority].head != NULL && rqstByPriority[priority].msgCount > 0)
		{	
			msgItem *prevMsg = NULL;
			msgItem *currMsg = rqstByPriority[priority].head;
			
			while(currMsg != NULL && (currMsg->msg)[(currMsg->msg).length() - 1] != END_TX_CHAR)
			{
				prevMsg = currMsg;
				currMsg = currMsg->next;
			}
			
			if (currMsg != NULL)
			{                
                retMsgItem = *currMsg;
                
				if (currMsg == rqstByPriority[priority].head)
				{
					rqstByPriority[priority].head = currMsg->next;
				}
				else
				{
					prevMsg->next = currMsg->next;
				}
				
				delete currMsg;
				--(rqstByPriority[priority].msgCount);
			}
			
		}
	}
	xTaskResumeAll();

    return retMsgItem;
}


/*
 * Function that return the string of the oldest response for the selected priority and taskID.
 */
String getResponse(uint16_t priority, uint16_t taskID)
{
	String retStr = "";
    vTaskSuspendAll();
	{
		if (responseByPriority[priority].head == NULL || responseByPriority[priority].msgCount == 0)
		{
			retStr = "";
		}
		else
		{	
			msgItem *prevMsg = NULL;
			msgItem *currMsg = responseByPriority[priority].head;
			
			while(currMsg != NULL && currMsg->taskID != taskID)
			{
				prevMsg = currMsg;
				currMsg = currMsg->next;
			}
			
			if (currMsg != NULL)
			{
				retStr = currMsg->msg;
                                
				if (currMsg == responseByPriority[priority].head)
				{
					responseByPriority[priority].head = currMsg->next;
				}
				else
				{
					prevMsg->next = currMsg->next;
				}
				
				delete currMsg;
				--(responseByPriority[priority].msgCount);
			}
			
		}
	}
	xTaskResumeAll();

    return retStr;
}


/*
 * only for debugging purposes!
 * example: printQueue(&rqstByPriority[pxGetCurrentTaskPriority()]);
 */
void printQueue(msgQueue *queue)
{
    vTaskSuspendAll();
    {
		Serial.print("msgCount in queue: ");
		Serial.println(queue->msgCount);
		Serial.println("------------------");
		int i = 0;
		msgItem *msg = queue->head;
		while (msg != NULL)
		{
			Serial.print("[");
			Serial.print(i++);
			Serial.print("] : taskID = ");
			Serial.print(msg->taskID);
			Serial.print(" msg = ");
			Serial.print(msg->msg);
			Serial.print(" next = ");
			Serial.println((long)msg->next);
			msg = msg->next;
		}
		Serial.println("fine coda\n\n");
	}
	xTaskResumeAll();
}
 


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

#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
extern void commTask(void *pvParameters);
static TaskHandle_t commTaskHandle;

void ArancinoClass::startScheduler() {
    vSetErrorLed(LED_BUILTIN, HIGH);
    xTaskCreate(commTask,     "Communication task",       256, (void *)&stream, configMAX_PRIORITIES - 1, &commTaskHandle);

    vTaskStartScheduler();
}
#endif


void ArancinoClass::setReservedCommunicationMode(int mode){
	COMM_MODE = mode;
}

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
    String message = receiveArancinoResponse(END_TX_CHAR);
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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
	sendArancinoCommand(DATA_SPLIT_CHAR);
	sendArancinoCommand(value);
	sendArancinoCommand(END_TX_CHAR);
	String message = receiveArancinoResponse(END_TX_CHAR);

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

void ArancinoClass::set( String key, uint32_t value ) {
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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
    String message = receiveArancinoResponse(END_TX_CHAR);
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
	String message = receiveArancinoResponse(END_TX_CHAR);
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
	String message = receiveArancinoResponse(END_TX_CHAR);
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
	String message = receiveArancinoResponse(END_TX_CHAR);
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
    String message = receiveArancinoResponse(END_TX_CHAR);
	return parse(message).toInt();
}

int ArancinoClass::hset( String key, String field, int value ) {
	hset(key, field, String(value));
}

int ArancinoClass::hset( String key, String field, double value ) {
	hset(key, field, String(value));
}

int ArancinoClass::hset( String key, String field, uint32_t value ) {
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
    String message = receiveArancinoResponse(END_TX_CHAR);
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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
		case SYNCH:
			_set(key, value);
		break;

		case ASYNCH:
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
	String message = receiveArancinoResponse(END_TX_CHAR);

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
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		insertRequest(USE_PRIORITIES * pxGetCurrentTaskPriority(), pxGetCurrentTaskNumber(), (String)command);
	}
	else
	{
		stream.print(command);
	}
#else
	stream.print(command);
#endif

#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(command);
	}
#endif
}

void ArancinoClass::sendArancinoCommand(char command){
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		insertRequest(USE_PRIORITIES * pxGetCurrentTaskPriority(), pxGetCurrentTaskNumber(), (String)command);
	}
	else
	{
		stream.print(command);
	}
#else
	stream.print(command);
#endif

#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		if(command == END_TX_CHAR)
			Serial.println(command);
		else
			Serial.print(command);
	}
#endif
}

/*
 * 'terminator' char is used only for non-freeRTOS implementations.
 * For freeRTOS implementations is always used END_TX_CHAR as terminator char (see commTask()).
 */
String ArancinoClass::receiveArancinoResponse(char terminator)
{
    String response = "";
 #if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
        uint16_t taskID = pxGetCurrentTaskNumber();
        response = getResponse(USE_PRIORITIES && pxGetCurrentTaskPriority(), taskID);
    }
    else
    {
        response = stream.readStringUntil(terminator);
    }
#else
    response = stream.readStringUntil(terminator);
#endif  
    return response;
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
