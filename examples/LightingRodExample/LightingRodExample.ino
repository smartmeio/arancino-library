#include <LightningRod.h>

void setup() {
  
  LRSerial.begin();
}

void loop() {

  String response = LRSerial.set("press","30");
  response = LRSerial.get("temp");
  delay(5000);

}
