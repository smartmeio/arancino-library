#include <Arancino.h>
#define LED_BUILTIN 13
TaskHandle_t task1Handle;


ArancinoMetadata amdata = {
  .fwname = "FreeRTOS Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000"
};

void setup() {

  Arancino.begin(amdata);
  pinMode(LED_BUILTIN, OUTPUT);
  SERIAL_DEBUG.begin(115200);

  xTaskCreate(loop2,     "Task 1",       256, NULL, tskIDLE_PRIORITY, &task1Handle);
  Arancino.startScheduler();

}

void loop() {

    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

}
