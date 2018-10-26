#include <Arancino.h>

void setup() {

  Arancino.begin();

}

void loop() {

  int hset_code = Arancino.hset("CAP","Messina","98100");
  hset_code = Arancino.hset("CAP","Torregrotta","98040");
  hset_code = Arancino.hset("CAP","Roma","00100");
  delay(5000);
  String hget_value = Arancino.hget("CAP","Messina");
  String* hgetall_value = Arancino.hgetall("CAP");
  delay(5000);
  String* hgetkeys_value = Arancino.hgetkeys("CAP");
  String* hgetvals_value = Arancino.hgetvals("CAP");
  delay(5000);
  int hdel_value=Arancino.hdel("CAP","Roma");
  hdel_value=Arancino.hdel("CAP","Torino");
  delay(5000);

}
