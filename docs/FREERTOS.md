## FreeRTOS implementation
The following section concern the use of FreeRTOS, already implemented in the Arancino Library; will be illustrated the Arancino-related FreeRTOS APIs and some basic FreeRTOS functions. For more advanced features, please see the official [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html).

### FreeRTOS library
The Arancino library use a [FreeRTOS porting](https://github.com/BriscoeTech/Arduino-FreeRTOS-SAMD21) for SAMD architecture that can be installed directly from the official Arduino library repository through the library manager. __User must install the correct FreeRTOS library__; the correct function it's not guaranteed with other FreeRTOS libraries. User doesn't have to include the FreeRTOS library because it is already included in Arancino library.

### Notes about using FreeRTOS on other Arancino architectures
The Arancino library currently supports 3 other architectures. 
| Board       | Architecture     | FreeRTOS support|
|--------------|-----------|------------|
| Arancino Volante | NRF52     | yes     |
| Arancino Pico      | RP20| no     |
| Arancino V   | STM32| yes    |
___
### startScheduler
##### *void startScheduler*
Configure the `LED_BUILTIN` for debug and start the FreeRTOS scheduler. When a fatal FreeRTOS error occur, the `LED_BUILTIN` will blink following this codes:

-    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
-    2 blinks  - Stack overflow, task needs more bytes defined for its stack.
-    1 blink - Malloc failed, probably ran out of heap.

> **IMPORTANT**: `Arancino.startScheduler()` never return, so **MUST be** the last instruction of Arduino begin() function.

##### Example:
```c++
#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "00.1 - Start Scheduler Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

void setup() {
  Arancino.begin(amdata);
  Arancino.startScheduler();
}

void loop() {
    //do something
}
```

### Basic FreeRTOS APIs
___
### xTaskCreate
##### BaseType_t xTaskCreate(TaskFunction_t pvTaskCode, const char &ast;pcName, configSTACK_DEPTH_TYPE usStackDepth, void &ast;pvParameters, UBaseType_t uxPriority, TaskHandle_t &ast; pxCreatedTask);

Create a new task and add it to the list of tasks that are ready to run.

##### Parameters
* **`pvTaskCode`**: Pointer to the task entry function (just the name of the function that implements the task, see the example below).
Tasks are normally implemented as an infinite loop, and must never attempt to return or exit from their implementing function. Tasks can however [delete themselves](https://www.freertos.org/a00126.html).

* **`pcName`**:	A descriptive name for the task. This is mainly used to facilitate debugging, but can also be used to [obtain a task handle](https://www.freertos.org/a00021.html#xTaskGetHandle).
The maximum length of a task’s name is set using the config `MAX_TASK_NAME_LEN` parameter in `FreeRTOSConfig.h`.

* **`usStackDepth`**:	The number of words (not bytes!) to allocate for use as the task’s stack. For example, if the stack is 16-bits wide and _usStackDepth_ is 100, then 200 bytes will be allocated for use as the task’s stack. As another example, if the stack is 32-bits wide and _usStackDepth_ is 400 then 1600 bytes will be allocated for use as the task’s stack.
The stack depth multiplied by the stack width must not exceed the maximum value that can be contained in a variable of type `size_t`. See the FAQ [How big should the stack be?](https://www.freertos.org/FAQMem.html#StackSize).

* **`pvParameters`**:	A value that will passed into the created task as the task’s parameter.
If _pvParameters_ is set to the address of a variable then the variable must still exist when the created task executes – so it is not valid to pass the address of a stack variable.

* **`uxPriority`**:	The [priority](https://www.freertos.org/RTOS-task-priority.html) at which the created task will execute.

* **`pxCreatedTask`**: Used to pass a [handle](https://www.freertos.org/a00019.html#xTaskHandle) to the created task out of the xTaskCreate() function. pxCreatedTask is optional and can be set to NULL.

##### Return value
BaseType_t return: If the task was created successfully then `pdPASS` is returned. Otherwise `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` is returned.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    Arancino.set("foo", "bar");
    vTaskDelay(2000); //wait 2 seconds (non-blocking delay)
  }
}

void setup() {
	SERIAL_DEBUG.begin(115200);
  Arancino.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
```
___
### vTaskDelay
##### void vTaskDelay(const TickType_t xTicksToDelay);
Delay a task for a given number of ticks. The actual time that the task remains blocked depends on the tick rate. The constant `portTICK_PERIOD_MS` can be used to calculate real time from the tick rate – with the resolution of one tick period.

##### Parameters
* **`xTicksToDelay`**: The amount of time, in tick periods, that the calling task should block. If, as in this implementation, the tick rate is 1000hz, the `portTICK_PERIOD_MS` is `1` so `xTicksToDelay` corresponds to milliseconds.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    vTaskDelay(1000); //wait 1 second (non-blocking delay)
  }
}

void setup() {
  Arancino.begin();
  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```
___
### vTaskSuspend
##### void vTaskSuspend(TaskHandle_t xTaskToSuspend);
Suspend any task. When suspended a task will never get any microcontroller processing time, no matter what its priority. Calls to vTaskSuspend are not accumulative – i.e. calling vTaskSuspend() twice on the same task still only requires one call to vTaskResume() to ready the suspended task.

##### Parameters
* **`xTaskToSuspend`**: Handle to the task being suspended. Passing a NULL handle will cause the calling task to be suspended.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    vTaskSuspend(loop2Handle); //executed only once - never resumed.
  }
}

void setup() {
  Arancino.begin();
  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```

___
### vTaskResume
##### void vTaskResume(TaskHandle_t xTaskToResume);
Resumes a suspended task. A task that has been suspended by one or more calls to vTaskSuspend () will be made available for running again by a single call to vTaskResume ().

##### Parameters
* **`xTaskToSuspend`**: Handle to the task being readied.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;
int state = 0;

void loop2(void *pvPramaters)
{
  while (1)
  {
    state = !state;
    digitalWrite(LED_BUILTIN, state); //blink
    vTaskSuspend(loop2Handle);
  }
}

void loop3(void *pvPramaters)
{
  while (1)
  {
    vTaskResume(loop2Handle);
    vTaskDelay(1000);
  }
}

void setup() {
  Arancino.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  xTaskCreate(loop3,     "Loop 3",       256, NULL, tskIDLE_PRIORITY, NULL);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```



