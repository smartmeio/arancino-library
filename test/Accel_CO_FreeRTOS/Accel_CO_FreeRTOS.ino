/*
 * CO in slot 1
 * Accel in slot 2
 *
 * download SparkFun ADXL345 Library (library Manager)
 * edit line 27 of SparkFun_ADXL345.cpp
 * from: #define ADXL345_DEVICE (0x53)    // Device Address for ADXL345
 * to:   #define ADXL345_DEVICE (0x1D)    // Device Address for ADXL345
 *
 */

#include <Arancino.h>
#define LED_PIN 13

void setup() {
  Arancino.begin();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Arancino.flush();
  Arancino.set("CO", 0);

  Arancino.hset("accel", "x", "0");
  Arancino.hset("accel", "y", "0");
  Arancino.hset("accel", "z", "0");

  xTaskCreate(GetTask,     "Get task",       128, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(COTask,     "CO task",       128, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(AccelTask,     "Accel task",       128, NULL, tskIDLE_PRIORITY + 1, NULL);

  Arancino.startScheduler();
}


void loop() {
  String mem = "free memory: " + (String)xPortGetFreeHeapSize();
  Serial.println(mem);
  digitalWrite(LED_PIN, (millis() / 1000) % 2);
  
  delay(200);

}
