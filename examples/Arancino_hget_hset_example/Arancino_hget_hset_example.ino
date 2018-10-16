#include <Arancino.h>

void setup() {

  Arancino.begin();
 
}

void loop() {

  int hset_code = Arancino.hset("meteo","messina","pioggia");
  delay(100);
  String hget_value = Arancino.hget("meteo","messina");
  delay(100);
  hget_value = Arancino.hget("meteo","lipari");
  delay(5000);

}

