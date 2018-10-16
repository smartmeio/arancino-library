#include <Arancino.h>

void setup() {
  
  Arancino.begin();
}

void loop() {

  Arancino.set("press","30");
  String response = Arancino.get("temp");
  delay(5000);

}
