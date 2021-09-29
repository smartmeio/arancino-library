#include <Arancino.h>
#define LED_PIN 13
TaskHandle_t task1Handle;


ArancinoMetadata amdata = {
  .fwname = "FreeRTOS Example",
  .fwversion = "1.0.1",
  .tzoffset = "+1000" 
};

void setup() {
  
  Arancino.begin(amdata);
  pinMode(LED_PIN, OUTPUT);
  SERIAL_DEBUG.begin(115200);
  
  xTaskCreate(loop2,     "Task 1",       256, NULL, tskIDLE_PRIORITY, &task1Handle);
  Arancino.startScheduler();
  
}

void loop() {
    
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);

}
