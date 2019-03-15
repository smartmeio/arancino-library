#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);

}

void loop() {

  int hset_code = Arancino.hset("CAP","Messina","98100");
  hset_code = Arancino.hset("CAP","Torregrotta","98040");
  hset_code = Arancino.hset("CAP","Roma","00100");
  delay(5000);
  String hget_value = Arancino.hget("CAP","Messina");
  String* hgetall_val = Arancino.hgetall("CAP");
  for(int i=0;i<Arancino.getArraySize(hgetall_val);i+=2){
    Serial.print("key: ");
    Serial.print(*(hgetall_val+i));
    Serial.print(" | value: ");
    Serial.println(*(hgetall_val+i+1));
  }
  delay(5000);
  String* hkeys_value = Arancino.hkeys("CAP");
  String* hvals_value = Arancino.hvals("CAP");
  delay(5000);
  int hdel_value=Arancino.hdel("CAP","Roma");
  hdel_value=Arancino.hdel("CAP","Torino");
  delay(5000);

}
