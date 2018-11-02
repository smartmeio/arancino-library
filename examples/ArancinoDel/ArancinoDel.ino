#include <Arancino.h>

void setup() {

  Arancino.begin();

}

void loop() {

  Arancino.set("pressure",1023);
  Arancino.set("humidity",67.5);
  Arancino.set("meteo","sole");
  Arancino.set("temperature","24");

  int a = Arancino.del("pressure");
  String* key= Arancino.keys();

  delay(5000);

}
