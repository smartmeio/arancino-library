#include <Arancino.h>

bool search(char** array, char* str, uint cnt) {
  for (uint i = 0; i < cnt; i ++) {
    if (strcmp(array[i], str) == 0) return true;
  }

  return false;
}

void unit_set() {
    ArancinoPacket response;

    Serial1.println("------ ARANCINO SET TEST ------");
    response = Arancino.set("unit_set_int", 5);
    
    if (response.isError) {
        Serial1.print("SET INT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET INT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET INT: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET INT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET INT: OK");


    response = Arancino.set("unit_set_int_pers", 5, true);
    
    if (response.isError) {
        Serial1.print("SET INT PERS: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET INT PERS: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET INT PERS: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET INT PERS: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET INT PERS: OK");


    response = Arancino.set("unit_set_double", 5.5);
    
    if (response.isError) {
        Serial1.print("SET DOUBLE: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET DOUBLE: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET DOUBLE: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET DOUBLE: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET DOUBLE: OK");


    response = Arancino.set("unit_set_double_pers", 5.5, true);
    
    if (response.isError) {
        Serial1.print("SET DOUBLE PERS: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET DOUBLE PERS: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET DOUBLE PERS: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET DOUBLE PERS: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET DOUBLE PERS: OK");


    response = Arancino.set("unit_set_long", (long)5);
    
    if (response.isError) {
        Serial1.print("SET LONG: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET LONG: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET LONG: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET LONG: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET LONG: OK");


    response = Arancino.set("unit_set_long_pers", (long)5, true);
    
    if (response.isError) {
        Serial1.print("SET LONG PERS: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET LONG PERS: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET LONG PERS: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET LONG PERS: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET LONG PERS: OK");


    response = Arancino.set("unit_set_str", "val");
    
    if (response.isError) {
        Serial1.print("SET STR: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET STR: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET STR: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET STR: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET STR: OK");


    response = Arancino.set("unit_set_str_pers", "val", true);
    
    if (response.isError) {
        Serial1.print("SET STR PERS: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET STR PERS: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET STR PERS: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET STR PERS: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET STR PERS: OK");


    response = Arancino.set("unit_set_uint32t", (u_int32_t)5);
    
    if (response.isError) {
        Serial1.print("SET UINT32T: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET UINT32T: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET UINT32T: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET UINT32T: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET UINT32T: OK");


    response = Arancino.set("unit_set_uint32t_pers", (u_int32_t)5, true);
    
    if (response.isError) {
        Serial1.print("SET UINT32T PERS: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("SET UINT32T PERS: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("SET UINT32T PERS: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("SET UINT32T PERS: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("SET UINT32T PERS: OK");
}

void unit_get() {
    Serial1.println("------ ARANCINO GET TEST ------");
    ArancinoPacket response;

    response = Arancino.get<ArancinoPacket>("unit_get_ok");

    if (response.isError) {
        Serial1.print("GET PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("GET PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING) {
        Serial1.print("GET PKT: FAIL (response.responseType should be STRING).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (strcmp(response.response.string, "ok") != 0) {
        Serial1.print("GET PKT: FAIL (response.response.string should be 'ok').");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("GET PKT: OK");

    response = Arancino.get<ArancinoPacket>("unit_get_ne");

    if (response.isError) {
        Serial1.print("GET NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("GET NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING) {
        Serial1.print("GET NE PKT: FAIL (response.responseType should be STRING).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("GET NE PKT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("GET NE PKT: OK");
}   

void unit_del() {
    Serial1.println("------ ARANCINO DEL TEST ------");
    ArancinoPacket response;

    response = Arancino.del<ArancinoPacket>("unit_del_ok");

    if (response.isError) {
        Serial1.print("DEL PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("DEL PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("DEL PKT: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 1) {
        Serial1.print("DEL PKT: FAIL (response.response.integer should be 1).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("DEL PKT: OK");

    response = Arancino.del<ArancinoPacket>("unit_del_ne");

    if (response.isError) {
        Serial1.print("DEL NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("DEL NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("DEL NE PKT: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("DEL NE PKT: FAIL (response.response.integer should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("DEL NE PKT: OK");
}

void unit_hset() {
    ArancinoPacket response;

    Serial1.println("------ ARANCINO HSET TEST ------");
    response = Arancino.hset("unit_hset_int", "test", 5);
    
    if (response.isError) {
        Serial1.print("HSET INT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 101) {
        Serial1.print("SHET INT: FAIL (response.responseCode should be 101).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("HSET INT: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HSET INT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HSET INT: OK");


    response = Arancino.hset("unit_hset_double", "test", 5.5);
    
    if (response.isError) {
        Serial1.print("HSET DOUBLE: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 101) {
        Serial1.print("HSET DOUBLE: FAIL (response.responseCode should be 101).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("HSET DOUBLE: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HSET DOUBLE: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HSET DOUBLE: OK");

    response = Arancino.hset("unit_hset_long", "test", (long)5);
    
    if (response.isError) {
        Serial1.print("HSET LONG: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 101) {
        Serial1.print("HSET LONG: FAIL (response.responseCode should be 101).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("HSET LONG: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HSET LONG: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HSET LONG: OK");

    response = Arancino.hset("unit_hset_str", "test", "val");
    
    if (response.isError) {
        Serial1.print("HSET STR: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 101) {
        Serial1.print("HSET STR: FAIL (response.responseCode should be 101).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("HSET STR: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HSET STR: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HSET STR: OK");

    response = Arancino.hset("unit_hset_uint32t", "test", (u_int32_t)5);
    
    if (response.isError) {
        Serial1.print("HSET UINT32T: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 101) {
        Serial1.print("HSET UINT32T: FAIL (response.responseCode should be 101).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != VOID) {
        Serial1.print("HSET UINT32T: FAIL (response.responseType should be VOID).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HSET UINT32T: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HSET UINT32T: OK");
}

void unit_hget() {
    Serial1.println("------ ARANCINO HGET TEST ------");
    ArancinoPacket response;

    response = Arancino.hget<ArancinoPacket>("unit_hget_ok", "test");

    if (response.isError) {
        Serial1.print("HGET PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HGET PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING) {
        Serial1.print("HGET PKT: FAIL (response.responseType should be STRING).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (strcmp(response.response.string, "ok") != 0) {
        Serial1.print("HGET PKT: FAIL (response.response.string should be 'ok').");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HGET PKT: OK");

    response = Arancino.hget<ArancinoPacket>("unit_hget_ne", "test");

    if (response.isError) {
        Serial1.print("HGET NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HGET NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING) {
        Serial1.print("HGET NE PKT: FAIL (response.responseType should be STRING).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.string != NULL) {
        Serial1.print("HGET NE PKT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(response.response.string);
    } else Serial1.println("HGET NE PKT: OK");
}

void unit_hgetall() {
    Serial1.println("------ ARANCINO HGETALL TEST ------");
    ArancinoPacket response;

    response = Arancino.hgetall<ArancinoPacket>("unit_h_ok");

    if (response.isError) {
        Serial1.print("HGETALL PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HGETALL PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HGETALL PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (((int)*(response.response.stringArray - 1)) != 6) {
        Serial1.print("HGETALL PKT: FAIL (response.response.stringArray len should be 6).");
        Serial1.print("Value: ");
        Serial1.println((int)*(response.response.stringArray - 1));
    } else if (!search(response.response.stringArray, "test1", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find test1 in string array).");
    } else if (!search(response.response.stringArray, "test2", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find test2 in string array).");
    } else if (!search(response.response.stringArray, "test3", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find test3 in string array).");
    } else if (!search(response.response.stringArray, "val1", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find val1 in string array).");
    } else if (!search(response.response.stringArray, "val2", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find val2 in string array).");
    } else if (!search(response.response.stringArray, "val3", 6)) {
        Serial1.print("HGETALL PKT: FAIL (can't find val3 in string array).");
    } else Serial1.println("HGETALL PKT: OK");


    response = Arancino.hgetall<ArancinoPacket>("unit_h_ne");

    if (response.isError) {
        Serial1.print("HGETALL NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HGETALL NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HGETALL NE PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.stringArray != NULL) {
        Serial1.print("HGETALL NE PKT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(*(response.response.stringArray));
    } else Serial1.println("HGETALL NE PKT: OK");
}

void unit_hkeys() {
  Serial1.println("------ ARANCINO HKEYS TEST ------");
    ArancinoPacket response;

    response = Arancino.hkeys<ArancinoPacket>("unit_h_ok");

    if (response.isError) {
        Serial1.print("HKEYS PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HKEYS PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HKEYS PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (((int)*(response.response.stringArray - 1)) != 3) {
        Serial1.print("HKEYS PKT: FAIL (response.response.stringArray len should be 3).");
        Serial1.print("Value: ");
        Serial1.println(((int)*(response.response.stringArray - 1)));
    } else if (!search(response.response.stringArray, "test1", 3)) {
        Serial1.print("HKEYS PKT: FAIL (can't find test1 in string array).");
    } else if (!search(response.response.stringArray, "test2", 3)) {
        Serial1.print("HKEYS PKT: FAIL (can't find test2 in string array).");
    } else if (!search(response.response.stringArray, "test3", 3)) {
        Serial1.print("HKEYS PKT: FAIL (can't find test3 in string array).");
    } else Serial1.println("HKEYS PKT: OK");


    response = Arancino.hkeys<ArancinoPacket>("unit_h_ne");

    if (response.isError) {
        Serial1.print("HKEYS NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HKEYS NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HKEYS NE PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.stringArray != NULL) {
        Serial1.print("HKEYS NE PKT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(*(response.response.stringArray));
    } else Serial1.println("HKEYS NE PKT: OK");
}

void unit_hvals() {
  Serial1.println("------ ARANCINO HVALS TEST ------");
    ArancinoPacket response;

    response = Arancino.hvals<ArancinoPacket>("unit_h_ok");

    if (response.isError) {
        Serial1.print("HVALS PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HVALS PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HVALS PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (((int)*(response.response.stringArray - 1)) != 3) {
        Serial1.print("HVALS PKT: FAIL (response.response.stringArray len should be 3).");
        Serial1.print("Value: ");
        Serial1.println(((int)*(response.response.stringArray - 1)));
    } else if (!search(response.response.stringArray, "val1", 3)) {
        Serial1.print("HVALS PKT: FAIL (can't find val1 in string array).");
    } else if (!search(response.response.stringArray, "val2", 3)) {
        Serial1.print("HVALS PKT: FAIL (can't find val2 in string array).");
    } else if (!search(response.response.stringArray, "val3", 3)) {
        Serial1.print("HVALS PKT: FAIL (can't find val3 in string array).");
    } else Serial1.println("HVALS PKT: OK");


    response = Arancino.hvals<ArancinoPacket>("unit_h_ne");

    if (response.isError) {
        Serial1.print("HVALS NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HVALS NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("HVALS NE PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.stringArray != NULL) {
        Serial1.print("HVALS NE PKT: FAIL (response.response.string should be NULL).");
        Serial1.print("Value: ");
        Serial1.println(*(response.response.stringArray));
    } else Serial1.println("HVALS NE PKT: OK");
}

void unit_hdel() {
  Serial1.println("------ ARANCINO HDEL TEST ------");
    ArancinoPacket response;

    response = Arancino.hdel<ArancinoPacket>("unit_hdel_ok", "test");

    if (response.isError) {
        Serial1.print("HDEL PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HDEL PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("HDEL PKT: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 1) {
        Serial1.print("HDEL PKT: FAIL (response.response.integer should be 1).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("HDEL PKT: OK");

    response = Arancino.hdel<ArancinoPacket>("unit_hdel_ne", "test");

    if (response.isError) {
        Serial1.print("HDEL NE PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("HDEL NE PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("HDEL NE PKT: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("HDEL NE PKT: FAIL (response.response.integer should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("HDEL NE PKT: OK");
}

void unit_keys() {
  Serial1.println("------ ARANCINO KEYS TEST ------");
    ArancinoPacket response;

    response = Arancino.keys<ArancinoPacket>("unit_keys_*");

    if (response.isError) {
        Serial1.print("KEYS PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("KEYS PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("KEYS PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (((int)*(response.response.stringArray - 1)) != 3) {
        Serial1.print("KEYS PKT: FAIL (response.response.stringArray len should be 3).");
        Serial1.print("Value: ");
        Serial1.println(((int)*(response.response.stringArray - 1)));
    } else if (!search(response.response.stringArray, "unit_keys_a", 3)) {
        Serial1.print("KEYS PKT: FAIL (can't find unit_keys_a in string array).");
    } else if (!search(response.response.stringArray, "unit_keys_b", 3)) {
        Serial1.print("KEYS PKT: FAIL (can't find unit_keys_b in string array).");
    } else if (!search(response.response.stringArray, "unit_keys_c", 3)) {
        Serial1.print("KEYS PKT: FAIL (can't find unit_keys_c in string array).");
    } else Serial1.println("KEYS PKT: OK");
}

void unit_publish() {
    ArancinoPacket response;

    Serial1.println("------ ARANCINO PUBLISH TEST ------");
    response = Arancino.publish("unit_pub_int", 5);
    
    if (response.isError) {
        Serial1.print("PUBLISH INT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("PUBLISH INT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("PUBLISH INT: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("PUBLISH INT: FAIL (response.response.string should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("PUBLISH INT: OK");   

    response = Arancino.publish("unit_pub_double", 5.5);
    
    if (response.isError) {
        Serial1.print("PUBLISH DOUBLE: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("PUBLISH DOUBLE: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("PUBLISH DOUBLE: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("PUBLISH DOUBLE: FAIL (response.response.string should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("PUBLISH DOUBLE: OK");   

    response = Arancino.publish("unit_pub_long", (long)5);
    
    if (response.isError) {
        Serial1.print("PUBLISH LONG: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("PUBLISH LONG: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("PUBLISH LONG: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("PUBLISH LONG: FAIL (response.response.string should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("PUBLISH LONG: OK");   

    response = Arancino.publish("unit_pub_str", "test");
    
    if (response.isError) {
        Serial1.print("PUBLISH STR: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("PUBLISH STR: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("PUBLISH STR: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("PUBLISH STR: FAIL (response.response.string should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("PUBLISH STR: OK");   

    response = Arancino.publish("unit_pub_uint32t", (uint32_t)5);
    
    if (response.isError) {
        Serial1.print("PUBLISH UINT32T: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("PUBLISH UINT32T: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != INT) {
        Serial1.print("PUBLISH UINT32T: FAIL (response.responseType should be INT).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (response.response.integer != 0) {
        Serial1.print("PUBLISH UINT32T: FAIL (response.response.string should be 0).");
        Serial1.print("Value: ");
        Serial1.println(response.response.integer);
    } else Serial1.println("PUBLISH UINT32T: OK");   
}

void unit_mset() {
  Serial1.println("------ ARANCINO MSET TEST ------");
  char* keys[] = {"unit_mset_a", "unit_mset_b", "unit_mset_c"};
  char* values[] = {"value1", "value2", "value3"};

  ArancinoPacket response;

  response = Arancino.mset(keys, values, 3);

  if (response.isError) {
      Serial1.print("MSET: FAIL (response.isError should be false).");
      Serial1.print("Value: ");
      Serial1.println(response.isError);
  } else if (response.responseCode != 100) {
      Serial1.print("MSET: FAIL (response.responseCode should be 100).");
      Serial1.print("Value: ");
      Serial1.println(response.responseCode);
  } else if (response.responseType != VOID) {
      Serial1.print("MSET: FAIL (response.responseType should be VOID).");
      Serial1.print("Value: ");
      Serial1.println(response.responseType);
  } else if (response.response.string != NULL) {
      Serial1.print("MSET: FAIL (response.response.string should be NULL).");
      Serial1.print("Value: ");
      Serial1.println(response.response.string);
  } else Serial1.println("MSET: OK");
}

void unit_mget() {
  Serial1.println("------ ARANCINO MGET TEST ------");
    ArancinoPacket response;

    char* keys[] = {"unit_mget_a", "unit_mget_b", "unit_mget_c"};

    response = Arancino.mget<ArancinoPacket>(keys, 3);

    if (response.isError) {
        Serial1.print("MGET PKT: FAIL (response.isError should be false).");
        Serial1.print("Value: ");
        Serial1.println(response.isError);
    } else if (response.responseCode != 100) {
        Serial1.print("MGET PKT: FAIL (response.responseCode should be 100).");
        Serial1.print("Value: ");
        Serial1.println(response.responseCode);
    } else if (response.responseType != STRING_ARRAY) {
        Serial1.print("MGET PKT: FAIL (response.responseType should be STRING_ARRAY).");
        Serial1.print("Value: ");
        Serial1.println(response.responseType);
    } else if (((int)*(response.response.stringArray - 1)) != 3) {
        Serial1.print("MGET PKT: FAIL (response.response.stringArray len should be 3).");
        Serial1.print("Value: ");
        Serial1.println(((int)*(response.response.stringArray - 1)));
    } else if (!search(response.response.stringArray, "val1", 3)) {
        Serial1.print("MGET PKT: FAIL (can't find val1 in string array).");
    } else if (!search(response.response.stringArray, "val2", 3)) {
        Serial1.print("MGET PKT: FAIL (can't find val2 in string array).");
    } else if (!search(response.response.stringArray, "val3", 3)) {
        Serial1.print("MGET PKT: FAIL (can't find val3 in string array).");
    } else Serial1.println("MGET PKT: OK");
}

void setup() {
    Serial1.begin(115200);
    Arancino.begin();

    unit_set(); 
    unit_get();
    unit_del();
    unit_hset();
    unit_hget();
    unit_hgetall();
    unit_hkeys();
    unit_hvals();
    unit_hdel();
    unit_keys();
    unit_publish();
    unit_mset();
    unit_mget();
    // char* keys[] = {"key1", "key2", "key3"};
    // char* values[] = {"value1", "value2", "value3"};

    // Arancino.mset(keys, values);
    // Arancino.mget(keys);
    // Arancino.mget<ArancinoPacket>(keys);

    // Arancino.flush();

}

void loop() {

}