#include <Arancino.h>

void setup() {
    Arancino.begin();

    Arancino.set("test", 5);
    Arancino.set("test", 5, true);
    Arancino.set("test", 5.5);
    Arancino.set("test", 5.5, true);
    Arancino.set("test", (long)5);
    Arancino.set("test", (long)5, true);
    Arancino.set("test", "val");
    Arancino.set("test", "val", true);
    Arancino.set("test", (uint32_t)5);
    Arancino.set("test", (uint32_t)5, true);

    Arancino.get("test");
    Arancino.get<ArancinoPacket>("test");

    Arancino.del("test");
    Arancino.del<ArancinoPacket>("test");

    Arancino.hset("test", "testf", 5);
    Arancino.hset("test", "testf", 5);
    Arancino.hset("test", "testf", 5.5);
    Arancino.hset("test", "testf", 5.5);
    Arancino.hset("test", "testf", (long)5);
    Arancino.hset("test", "testf", (long)5);
    Arancino.hset("test", "testf", "val");
    Arancino.hset("test", "testf", "val");
    Arancino.hset("test", "testf", (uint32_t)5);
    Arancino.hset("test", "testf", (uint32_t)5);

    Arancino.hget("test", "testf");
    Arancino.hget<ArancinoPacket>("test", "testf");

    Arancino.hgetall("test");
    Arancino.hgetall<ArancinoPacket>("test");

    Arancino.hkeys("test");
    Arancino.hkeys<ArancinoPacket>("test");

    Arancino.hvals("test");
    Arancino.hvals<ArancinoPacket>("test");

    Arancino.hdel("test", "testf");
    Arancino.hdel<ArancinoPacket>("test", "testf");

    Arancino.keys();
    Arancino.keys("a");
    Arancino.keys<ArancinoPacket>();
    Arancino.keys<ArancinoPacket>("a");

    Arancino.publish("test", 5);
    Arancino.publish("test", 5.5);
    Arancino.publish("test", (long)5);
    Arancino.publish("test", "val");
    Arancino.publish("test", (uint32_t)5);

    Arancino.flush();

}

void loop() {

}