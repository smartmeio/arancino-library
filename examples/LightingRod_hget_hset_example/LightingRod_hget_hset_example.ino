#include <LightningRod.h>

void setup() {

  LRSerial.begin();
  Serial.begin(115200);
  delay(1000);
 
}

void loop() {

  int hset_code = LRSerial.hset("meteo","messina","pioggia");
  Serial.print("del press: ");
  Serial.println(hset_code);
  delay(100);
  String hget_value = LRSerial.hget("meteo","messina");
  Serial.print("meteo messina: ");
  Serial.println(hget_value);
  delay(100);
  hget_value = LRSerial.hget("meteo","lipari");
  Serial.print("meteo lipari: ");
  Serial.println(hget_value);
  delay(5000);

}

