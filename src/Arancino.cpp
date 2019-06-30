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
#define STOP(x) Serial.println(x); delay(1000)

/*
 * Element of the queue that contain the requests to be sent through the UART.
 * One struct contain only one Arancino request and the task id from which it comes.
 */
typedef struct _msgItem
{
    char* msg;
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

int insertRequest(uint16_t, uint32_t, char*);
msgItem getRequest(uint16_t);
int getRqstCount(uint16_t);

int insertResponse(uint16_t, uint32_t, char*);
char* getResponse(uint16_t, uint16_t);
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
	char* response = NULL;

    while(1)
    {
        //Priorities system currently not implemented
		if (getRqstCount(USE_PRIORITIES) > 0)
		{
            msgItem rqst = getRequest(USE_PRIORITIES);
            (*stream).print(rqst.msg);
            free(rqst.msg);
        
            if (response != NULL)
            {
                //free(response);
                response = NULL;
            }
            
            long previousMillis = millis();
            
            while (millis() - previousMillis < TIMEOUT)
            {
                if ((*stream).available())
                {
                    char c = (*stream).read();
                    
                    if (response == NULL)
                    {
                        response = (char *)calloc(2, sizeof(char));
                        response[0] = c;
                        response[1] = '\0';
                    }
                    else
                    {
                        int oldLength = strlen(response); //'\0' non included
                        char* temp = (char *)calloc((oldLength + 2), sizeof(char)); // 2 = new char + '\0'
                        strcpy(temp, response);
                        free(response);
                        response = temp;
                        response[oldLength] = c;
                        response[oldLength + 1] = '\0';
                    }
                    
                    if (c == END_TX_CHAR)
                        break;   
                    
                }
            }
#if defined(DEBUG) && DEBUG == 1
            Serial.print("received response: ");
            for (int i = 0; i < strlen(response); i++)
            {
                if (response[i] < 32)
                {
                    Serial.print("|");
                    Serial.print(response[i], DEC);
                    Serial.print("|");
                }
                else
                    Serial.print(response[i]);
            }
            Serial.println("");
#endif
            insertResponse(USE_PRIORITIES, rqst.taskID, response);
            //vTaskResume(rqst.taskHandle); //waking up the task that made the request BUG -> not used
            //xTaskResumeAll();
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
int insertRequest(uint16_t priority, uint32_t taskID, char* msg)
{
    int toBeSuspended = 0;
    
    vTaskSuspendAll();
    msgItem *selectedMsg = NULL;
    
    if (rqstByPriority[priority].head == NULL)
    {
        rqstByPriority[priority].head = new msgItem;
        if (rqstByPriority[priority].head != NULL) //successfully allocated
        {
            (rqstByPriority[priority].head)->msg = NULL;
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
        
        while (currMsg != NULL && (currMsg->taskID != taskID || (currMsg->msg)[strlen(currMsg->msg) - 1] == END_TX_CHAR))
        {
            prevMsg = currMsg;
            currMsg = currMsg->next;
        }
        
        if (currMsg == NULL) //coda terminata
        {
            msgItem *temp = new msgItem;
            temp->msg = NULL;
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
    
    if (selectedMsg != NULL && strcmp(msg, "") != 0)
    {
        //int index = msg.indexOf(END_TX_CHAR); //-1 = carattere non trovato
        int index;
        char* charAddr = strchr(msg, END_TX_CHAR); //address (in ram) of the END_TX_CHAR in msg string 
        
        if (charAddr != NULL)
        {
            index = charAddr - msg;
        }
        else
        {
            index = -1;   
        }
    
        if (index > -1)
        {
            toBeSuspended = 1;
            //flag the task as toBeSuspended
            selectedMsg->taskHandle = xTaskGetCurrentTaskHandle();

            int oldLength = -1;
            if (selectedMsg->msg != NULL)
            {
               // while((selectedMsg->msg)[++oldLength] != '\0');
                oldLength = strlen(selectedMsg->msg);
            }
            else
            {
                (selectedMsg->msg) = (char*)malloc(sizeof(char));
                *(selectedMsg->msg) = '\0';   
                oldLength = 0;
            }

            
            char* newStr = (char *)calloc((oldLength + index + 2), sizeof(char));
            if (selectedMsg->msg != NULL)
            {
                strncpy(newStr, selectedMsg->msg, oldLength);
                newStr[oldLength] = '\0';
            }
            strncpy(&newStr[oldLength] , msg, index + 1);
            newStr[oldLength + index + 1] = '\0';
            
            free(selectedMsg->msg);
            selectedMsg->msg = newStr;

            ++(rqstByPriority[priority].msgCount);
            if (msg[index + 1] != '\0')
            {
                insertRequest(priority, taskID, &msg[index + 1]);
            }
        }
        else
        {
            int oldLength = -1;
            if (selectedMsg->msg != NULL)
            {
                oldLength = strlen(selectedMsg->msg);
            }
            else
            {
                (selectedMsg->msg) = (char*)malloc(sizeof(char));
                *(selectedMsg->msg) = '\0';   
                oldLength = 0;
            }
            
            int newLength = -1;
            if (msg != NULL)
            {
                newLength = strlen(msg);
            }
            
            char* newStr = (char *)calloc((oldLength + newLength + 2), sizeof(char));
            
            if (selectedMsg->msg != NULL)
            {
                strncpy(newStr, selectedMsg->msg, oldLength);
                newStr[oldLength] = '\0';
            }

            strncpy(&newStr[oldLength], msg, newLength);
            newStr[oldLength + newLength] = '\0';
            
            free(selectedMsg->msg);
            selectedMsg->msg = newStr;
        }
    }
    xTaskResumeAll();
    //task must be suspended here

    if (toBeSuspended)
    {
    /*
     * BUG il task viene sospeso nel momento sbagliato!
     * dopo xTaskResumeAll(); viene immediatamente eseguito commTask() che invia la richiesta sulla seriale e, una volta ricevuta la risposta,
     * sblocca il task corrente. Il problema è che con la prossima riga il task viene di nuovo bloccato e mai più sbloccato!
     */
        //vTaskSuspend(NULL);  // Suspend ourselves.
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
int insertResponse(uint16_t priority, uint32_t taskID, char* msg)
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
		
		if (selectedMsg != NULL && strcmp(msg, "") != 0)
		{
            selectedMsg->msg = msg; //caller must free
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
	{
        msgItem *response = responseByPriority[priority].head;
        while (response != NULL)
        {
            if (response->taskID == taskID)
                ++retVal;
            response = response->next;
        }
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
			
			while(currMsg != NULL && (currMsg->msg)[strlen(currMsg->msg) - 1] != END_TX_CHAR)
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
				
#warning "da controllare delete"
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
char* getResponse(uint16_t priority, uint16_t taskID)
{
	char* retStr = NULL;
    vTaskSuspendAll();
	{
		if (responseByPriority[priority].head == NULL || responseByPriority[priority].msgCount == 0)
		{
			retStr = NULL;
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
    strcpy(reservedKey[0], MONITOR_KEY);
    strcpy(reservedKey[1], LIBVERS_KEY);
    strcpy(reservedKey[2], MODVERS_KEY);
    strcpy(reservedKey[3], POWER_KEY);
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
    
    sendViaCOMM_MODE(LIBVERS_KEY, LIB_VERSION);
    
}

#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
extern void commTask(void *pvParameters);
static TaskHandle_t commTaskHandle;

void ArancinoClass::startScheduler() {
    vSetErrorLed(LED_BUILTIN, HIGH);
    xTaskCreate(commTask,     "Communication task",       1024, (void *)&stream, configMAX_PRIORITIES - 1, &commTaskHandle);

    vTaskStartScheduler();
}
#endif


void ArancinoClass::setReservedCommunicationMode(int mode){
	COMM_MODE = mode;
}

//============= API FUNCTIONS ======================

/*String ArancinoClass::get( String key ) {

}*/

char* ArancinoClass::get( char* key ) {
	if(isReservedKey(key)){
		return NULL;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(GET_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
    char* message = receiveArancinoResponse(END_TX_CHAR);
    return parse(message);
}

int ArancinoClass::del( char* key ) {

	if(isReservedKey(key)){
		return -1;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(DEL_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
    char* message = receiveArancinoResponse(END_TX_CHAR); //freed by parse()
    char* messageParsed = parse(message);

    int result = atoi(messageParsed);
    free(messageParsed);

	return result;
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

void ArancinoClass::_set( char* key, char* value ) {
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(SET_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(DATA_SPLIT_CHAR);
	sendArancinoCommand(value);
	sendArancinoCommand(END_TX_CHAR);

	char* message = receiveArancinoResponse(END_TX_CHAR);

	parse(message);
}

void ArancinoClass::set( char* key, char* value ) {

	if(isReservedKey(key)){
		//TODO maybe it's better to print a log
		return;
	}
	_set(key, value);

}

void ArancinoClass::set( char* key, int value ) {
    char str[20] = "";
    sprintf(str, "%d", value);
	set(key, str);
}

void ArancinoClass::set( char* key, double value ) {
    char str[50] = "";
    snprintf(str, 40, "%f", value); //TODO - BUG
    delay(1000);
	set(key, str);
}

void ArancinoClass::set( char* key, uint32_t value ) {
    char str[20] = "";
    sprintf(str, "%d", value);
	set(key, str);
}

char* ArancinoClass::hget( char* key, char* field ) {

	if(isReservedKey(key)){
		return "";
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HGET_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (strcmp(field, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	sendArancinoCommand(END_TX_CHAR);
	char* message = receiveArancinoResponse(END_TX_CHAR);

	return parse(message);
}

char** ArancinoClass::hgetall(char* key) {

	if(isReservedKey(key)){
		return NULL;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HGETALL_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
    char* message = receiveArancinoResponse(END_TX_CHAR);
    
	return parseArray(parse(message));
}


char** ArancinoClass::hkeys(char* key) {

	if(isReservedKey(key)){
		return NULL;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HKEYS_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	char* message = receiveArancinoResponse(END_TX_CHAR);
    
	return parseArray(parse(message));
}


char** ArancinoClass::hvals(char* key) {

	if(isReservedKey(key))
		return NULL;
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HVALS_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	sendArancinoCommand(END_TX_CHAR);
	char* message = receiveArancinoResponse(END_TX_CHAR);
	
	return parseArray(parse(message));
}


char** ArancinoClass::keys(char* pattern){
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(KEYS_COMMAND);					// send read request
	if (strcmp(pattern, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(pattern);
	}
	sendArancinoCommand(END_TX_CHAR);
	char* message = receiveArancinoResponse(END_TX_CHAR);
	
	return parseArray(parse(message));

}

int ArancinoClass::hset( char* key, char* field , char* value) {

	if(isReservedKey(key)){
		return -1;
	}
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HSET_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (strcmp(field, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	if (strcmp(value, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(value);
	}
	sendArancinoCommand(END_TX_CHAR);
    char* message = receiveArancinoResponse(END_TX_CHAR); //freed by parse()
    char* messageParsed = parse(message);

    int result = atoi(messageParsed);
    free(messageParsed);

	return result;
}


int ArancinoClass::hset( char* key, char* field, int value ) {
    char str[20]; 
    sprintf(str, "%d", value);
    hset(key, field, str);
}

int ArancinoClass::hset( char* key, char* field, double value ) {
	//hset(key, field, String(value)); //BUG - TODO
}

int ArancinoClass::hset( char* key, char* field, uint32_t value ) {
    char str[20]; 
    sprintf(str, "%d", value);
    hset(key, field, str);
}

int ArancinoClass::hdel( char* key, char* field ) {

	if(isReservedKey(key))
		return -1;

	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(HDEL_COMMAND);					// send read request
	if (strcmp(key, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(key);
	}
	if (strcmp(field, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(field);
	}
	sendArancinoCommand(END_TX_CHAR);
    char* message = receiveArancinoResponse(END_TX_CHAR); //freed by parse()
    char* messageParsed = parse(message);

    int result = atoi(messageParsed);
    free(messageParsed);

	return result;
}

int ArancinoClass::_publish( char* channel, char* msg ) {
	#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(SENT_STRING);
	}
	#endif
	sendArancinoCommand(PUBLISH_COMMAND);					// send read request
	if (strcmp(channel, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(channel);
	}
	if (strcmp(msg, "") != 0){
		sendArancinoCommand(DATA_SPLIT_CHAR);
		sendArancinoCommand(msg);
	}
	sendArancinoCommand(END_TX_CHAR);
    
    char* message = receiveArancinoResponse(END_TX_CHAR); //freed by parse()
    char* messageParsed = parse(message);

    int result = atoi(messageParsed);
    free(messageParsed);

	return result;
}

int ArancinoClass::publish( char* channel, char* msg ) {

	if(isReservedKey(channel)){
		//TODO maybe its better to log the error
		return -1;
	}
	return _publish(channel, msg);

}

int ArancinoClass::publish( int channel, char* msg ) {
    char str[20] = "";
    sprintf(str, "%d", channel);
	publish(str, msg);
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
	char* message = receiveArancinoResponse(END_TX_CHAR);

	parse(message);

}


int ArancinoClass::getArraySize(char** _array)
{
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  return dummy != NULL ? (int)(*dummy) : 0;
}

void ArancinoClass::freeArray(char** _array)
{
  char** dummy = (_array != NULL) ? _array - sizeof(char) : NULL;
  if (*_array != NULL)
    free(*_array);
  if (dummy != NULL)
    free(dummy);
}

//============= DEBUG FUNCTIONS ======================

void ArancinoClass::print(char* value){
	sendViaCOMM_MODE(MONITOR_KEY, value);
}

void ArancinoClass::print(String value){
	sendViaCOMM_MODE(MONITOR_KEY, value.begin());
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

void ArancinoClass::sendViaCOMM_MODE(char* key, char* value){
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


char** ArancinoClass::parseArray(char* data)
{
    char** arrayParsed = NULL;
    char* tempArray;

    char* previousDSCIndex = NULL;
    char* DSCIndex = data;
    
    int fieldCount = data != NULL;
    int maxLength = 0;
    
    //get the key count
    while (DSCIndex != NULL)
    {
        previousDSCIndex = DSCIndex;
        DSCIndex = strchr(DSCIndex + 1, DATA_SPLIT_CHAR);

        if (DSCIndex != NULL)
        {
            ++fieldCount;
            if (DSCIndex - previousDSCIndex > maxLength) //get the max key length
                maxLength = DSCIndex - previousDSCIndex;
        }
        else
        {
            maxLength = strlen(previousDSCIndex);
        }
    }

    if (data != NULL) {
        /*
         * Alloco un elemento in più (fieldCount + 1). 
         * Nel primo elemento inserisco la lunghezza del vettore;
         * Il valore di ritorno della funzione sarà l'elemento 1 del vettore (e non l'elemento 0)
         */
        arrayParsed = (char **)malloc((fieldCount + 1) * sizeof(char*)); //user must free!
        tempArray = (char *)malloc((fieldCount + 1) * (maxLength + 1) * sizeof(char)); //user must free!
        arrayParsed[0] = (char*)fieldCount;
        
    }
    
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

    return (arrayParsed != NULL) ? &arrayParsed[1] : NULL;
}




char* ArancinoClass::parse(char* message){

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

void ArancinoClass::sendArancinoCommand(String command){
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		insertRequest(USE_PRIORITIES * pxGetCurrentTaskPriority(), pxGetCurrentTaskNumber(), command.begin());
	}
	else
	{
#endif
		stream.print(command);
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	}
#endif

#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		Serial.print(command);
	}
#endif
}


void ArancinoClass::sendArancinoCommand(char* command){
    //command must terminate with '\0'!
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		insertRequest(USE_PRIORITIES * pxGetCurrentTaskPriority(), pxGetCurrentTaskNumber(), command);
	}
	else
	{
#endif
		stream.print(command);
#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
	}
#endif

#if defined(__SAMD21G18A__)
	if(!digitalRead(DBG_PIN)){
		if(command[strlen(command) - 1] == END_TX_CHAR)
			Serial.println(command);
		else
			Serial.print(command);
	}
#endif
}


void ArancinoClass::sendArancinoCommand(char command){
    char* c = (char *)malloc(sizeof(char)*2);
    c[0] = command;
    c[1] = '\0';
    sendArancinoCommand(c);
    free(c);
}


/*
 * 'terminator' char is used only for non-freeRTOS implementations.
 * For freeRTOS implementations is always used END_TX_CHAR as terminator char (see commTask()).
 */
char* ArancinoClass::receiveArancinoResponse(char terminator)
{
    char* response = NULL; //must be freed

 #if defined(__SAMD21G18A__) && defined(USEFREERTOS)

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
        uint16_t taskPriority = USE_PRIORITIES * pxGetCurrentTaskPriority();
        uint16_t taskID = pxGetCurrentTaskNumber();
        while (getResponseCount(taskPriority, taskID) == 0)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS); //TODO trovare una soluzione a questo obbrobrio
        }
        response = getResponse(taskPriority, taskID);
        
        
    }
    else
    {
#endif
        //BEGIN read from uart
        long previousMillis = millis();
        
        while (millis() - previousMillis < TIMEOUT)
        {
            if ((stream).available())
            {
                char c = (stream).read();

                if (response == NULL)
                {
                    response = (char *)calloc(2, sizeof(char));
                    response[0] = c;
                    response[1] = '\0';
                }
                else
                {
                    int oldLength = strlen(response);
                    char* temp = (char *)calloc((oldLength + 2), sizeof(char));
                    strcpy(temp, response);
                    free(response);
                    response = temp;
                    response[oldLength] = c;
                    response[oldLength + 1] = '\0';
                }
                
                if (c == terminator)
                    break;
            }
        }
        //END read from uart

#if defined(__SAMD21G18A__) && defined(USEFREERTOS)
    }
#endif  
    return response;
}

bool ArancinoClass::isReservedKey(char* key){
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

// Arancino instance
#if defined(__SAMD21G18A__)
SerialArancinoClass Arancino(SerialUSB);
#else
SerialArancinoClass Arancino(Serial);
#endif
//#endif
