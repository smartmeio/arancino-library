#include <Arancino.h>
#define LED_PIN 13
TaskHandle_t task1Handle;


void setup() {
  Arancino.begin();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  
  xTaskCreate(loop2,     "Task 1",       256, NULL, tskIDLE_PRIORITY, &task1Handle);
  Arancino.startScheduler();
}



void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
