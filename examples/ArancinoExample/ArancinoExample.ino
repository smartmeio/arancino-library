#include <Arancino.h>

void setup() {
  
  Arancino.begin();
  Arancino.set("temp",35);
  
}

void loop() {

  Arancino.set("press","30");
  String response = Arancino.get("temp");
  delay(1000);

}
