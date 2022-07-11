## API


> Note: when you get a value using api like `get`, `hget`, etc. please keep in mind to free memory each time using the `free` api.
>
> To prevent memory leak, code like this must be avoided:
> ```c++
> //Do not use
> Arancino.set("test", Arancino.get("mem"));
> ```
>
> Use the following instead:
> ```c++
> //Use this
> char* mem = Arancino.get("mem");
> Arancino.set("test", mem);
> Arancino.free(mem);
> ```


### metadata
#### *void metadata(ArancinoMetadata data)*  |  ***DEPRECATED from Version 1.4.0***
Stores the metadata to use later during the `START` command in `begin` API.

##### Parameters
* **`data`**: struct of type `ArancinoMetadata`. Represents the firmware name, version and local timezone offset.

##### Example
```c++
#include <Arancino.h>

ArancinoMetadata amdata = {
  .fwname = "Arancino Firmware",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};

void setup() {

  // deprecated: do not use the .metadata function directly. Use the mandatory metadata argument in the begin function instead.
  // This will be removed in the next major relase
  //Arancino.metadata(meta);

  //
  Arancino.begin(amdata);
}

void loop() {
  // Do something
}
```
___

### begin
##### *void begin(ArancinoMetadata amdata, bool useid = false, int timeout = TIMEOUT )*  |  ***DEPRECATED from Version 1.4.0***
##### *void begin(ArancinoMetadata amdata, ArancinoConfig aconfig)*

Starts the communication with the Arancino Module. When communication starts, the `begin` transmits the Arancino Library version for compatibility evaluatation.
From version `1.4.0` the `ArancinoMetadata` argument became mandatory, becouse the data contained in it is part of the initial trasmission.
From version

##### Parameters
- ArancinoConfig: is class composed only by attributes (see the following paragraph: [`Arancino Config`](#ArancinoConfig)):
  - `DECIMAL_DIGITS`
  - `SERIAL_TIMEOUT`
  - `PORT_ID_PREFIX_KEY`

The convenience of using class ArancinoConfig is to be able to increase the number of parameters without necessarily having to change the prototypes.
The use of the following arguments in the `begin` function is deprecated and will be removed in the next major release.
- **`timeout`**
- **`useid`**

___
### set
##### *ArancinoPacket set(char&ast; key, char&ast; value [, bool isPersistent] )*
##### *ArancinoPacket set(char&ast; key,  int value [, bool isPersistent] )*
##### *ArancinoPacket set(char&ast; key, float value [, bool isPersistent] )*
##### *ArancinoPacket set(char&ast; key, long value, [, bool isPersistent] )*
##### *ArancinoPacket set(char&ast; key, uint32_t value, [, bool isPersistent] )*

Set *key* to hold the string *value*. If *key* already holds a *value*, it is overwritten, regardless of its type.

##### Parameters
* **`key`**: the *key* name
* **`value`**: the *value* for the specified *key*. can be char*, int o float
* **`isPersistent`** optional boolean value to specify if value must be stored persistently or not. Default is `false`.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `VOID`;
  * `response`: `NULL`;


###### Note: [2](#notes), [5](#notes)

___
### get
##### *char&ast; get(char&ast; key)*
##### *ArancinoPacket get\<ArancinoPacket>(char&ast; key)*

Get the *value* of *key*. If the *key* does not exist, the special value `NULL` is returned.

##### Parameters
* **`key`**: the name of the key from which the value is retrieved

##### Return value
char&ast; reply:
- string that contains the value of selected *key*.
- NULL if the *key* doesn't exist.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING`;
  * `response.string`: `char*` pointer that can contain the value of selected *key* or `NULL` if the *key* doesn't exist.


###### Note: [1](#notes), [4](#notes)

___

### mset
##### *ArancinoPacket mset(char&ast;&ast; keys, char&ast;&ast; values, uint len)*
Executes the MSET command. Every key in the `keys` parameter is set to the corresponding value in the `values` parameter.

##### Parameters
* **`keys`**: keys to set;
* **`values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of `keys` array.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `VOID`;
  * `response`: `NULL`;

###### Note: [3](#notes)

### mget
##### *char&ast;&ast; mget(char&ast;&ast; keys, char&ast;&ast; values, uint len)*
##### *ArancinoPacket mget\<ArancinoPacket>(char&ast;&ast; keys, uint len)*

Retrieves multiple keys from redis.

##### Parameters
* **`keys`**: array containing keys to retrieve;
* **`len`**: number of keys to retrieve.

##### Return value
`get` function can return both `char**` and `ArancinoPacket` depending on the template used.

###### `char**`
In this case an array of `char*` in returned. Every element contains the value stored in the corresponding key in the `keys` parameter. If a key doesn't exist the corresponding element in the returned array is `NULL`.

###### `ArancinoPacket`
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringarray`: `char**` pointer that points to the start of the returned array of strings.


###### Note: [1](#notes), [4](#notes)

### del
##### *int del(char&ast; key )*
##### *ArancinoPacket del\<ArancinoPacket>(char&ast; key )*

Removes the specified *key*. A *key* is ignored if it does not exist.

##### Parameters
* **`key`**: the name of the key to delete.

##### Return value
int reply: The number of keys that were removed.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `INT`;
  * `response.integer`: The number of keys that were removed.


###### Note: [1](#notes)

___
### keys
##### *char&ast;&ast; keys(char&ast; pattern)*
##### *ArancinoPacket keys\<ArancinoPacket>(char&ast; pattern)*
Returns all the keys matching the *pattern*.

Supported glob-style patterns:

- h?llo matches hello, hallo and hxllo
- h*llo matches hllo and heeeello
- h[ae]llo matches hello and hallo, but not hillo
- h[^e]llo matches hallo, hbllo, ... but not hello
- h[a-b]llo matches hallo and hbllo

Use \ to escape special characters if you want to match them verbatim.

##### Parameters
* **`pattern`**: the pattern used to find matching keys.

##### Return value
char&ast;&ast; reply: list of keys matching *pattern*.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringArray`: `char**` pointer to the string array of keys matching *pattern*.


___
### hset
##### *ArancinoPacket hset(char&ast; key, char&ast; field , char&ast; value )*
##### *ArancinoPacket hset(char&ast; key, char&ast; field , int value )*
##### *ArancinoPacket hset(char&ast; key, char&ast; field , float value )*
##### *ArancinoPacket hset(char&ast; key, char&ast; field , long value )*
##### *ArancinoPacket hset(char&ast; key, char&ast; field , uint32_t value )*

Sets *field* in the hash stored at *key* with the specified *value*. If *key* does not exist, a new *key* holding a hash is created. If *field* already exists in the hash, it is overwritten.

##### Parameters
* **`key`**: the name of the *key* used to create the hash.
* **`field`**: the name of the *field* to store in the hash.
* **`value`**: the value to store in the hash with the specified field.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `VOID`;
  * `response`: `NULL`;


###### Note: [2](#notes)

___
### hget
##### *char&ast; hget(char&ast; key, char&ast; field )*
##### *ArancinoPacket hgetPacket\<ArancinoPacket>(char&ast; key, char&ast; field )*
Returns the *value* associated with *field* in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.
* **`field`**: the name of the *field* from which the value is retrieved

##### Return value
char&ast; reply:
- the *value* if a value is stored in *field* at *key*.
- NULL if there isn't a value stored.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING`;
  * `response.string`: `char*` pointer that can contain the *value* if a value is stored in *field* at *key* or `NULL` if there isn't a value stored.


###### Note: [1](#notes), [4](#notes)

___
### hgetall
##### *char&ast;&ast; hgetall(char&ast; key )*
##### *ArancinoPacket hgetall\<ArancinoPacket>(char&ast; key )*

Returns all fields and values of the hash stored at *key*. In the returned value, every *field* name is followed by its *value*.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
char&ast;&ast; reply: string array of field and value matching *key*.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringArray`: `char**` pointer that can contain the string array of field and value matching *key*

###### Note: [1](#notes), [4](#notes)

___
### hkeys
##### *char&ast;&ast; hkeys( String key )*
##### *ArancinoPacket hkeys\<ArancinoPacket>( String key )*
Returns all field names in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
char&ast;&ast; reply: string array of *fields* matching *key*.

or

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringArray`: `char**` pointer to the string array of *fields* matching *key*.


###### Note: [1](#notes), [4](#notes)

___
### hvals
##### *char&ast;&ast; hvals(char&ast; key )*
##### *ArancinoPacket hvals\<ArancinoPacket>(char&ast; key )*
Returns all values in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
char&ast;&ast; reply: string array of *values* matching *key*.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringArray`: `char**` pointer to the string array of *values* matching *key*.


###### Note: [1](#notes), [4](#notes)

___
### hdel
##### *int hdel(char&ast; key, char&ast; field )*
##### *ArancinoPacket hdel\<ArancinoPacket>(char&ast; key, char&ast; field )*
Removes the specified *field* from the hash stored at *key*. If *field* is specified and it does not exist within this hash, this command returns 0. If the key does not exist, it is treated as an empty hash and this command returns 0.


##### Parameters
* **`key`**: the name of the *key* stored in the hash.
* **`field`**: the name of the *field* stored in the hash at key to delete.

##### Return value
int reply:
- 1 if the *field* is removed from hash.
- 0 if the *field* or the *key* does not exist in the hash.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `INT`;
  * `response.integer`: 1 if the *field* is removed from hash or 0 if the *field* or the *key* does not exist in the hash.


###### Note: [1](#notes)

___
### flush
##### *ArancinoPacket flush()*
Delete all the keys.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `VOID`;
  * `response`: `NULL`.


___
### publish
##### *ArancinoPacket publish(char&ast; channel, char&ast; message)*
##### *ArancinoPacket publish(char&ast; channel, int message)*
##### *ArancinoPacket publish(char&ast; channel, float message)*
##### *ArancinoPacket publish(char&ast; channel, uint32_t message)*
##### *ArancinoPacket publish(char&ast; channel, long message)*

Posts a message to the given channel.


##### Parameters
* **`channel`**: the name of the *channel* where the message will be sent.
* **`message`**: *message* to send.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `INT`;
  * `response.integer`: the number of clients that received the message.


___
### store
##### *ArancinoPacket store(char&ast; key, int value)*
##### *ArancinoPacket store(char&ast; key, float value)*
##### *ArancinoPacket store(char&ast; key, long value)*
##### *ArancinoPacket store(char&ast; key, uint32_t value)*

##### *ArancinoPacket store(char&ast; key, int value, char&ast timestamp)*
##### *ArancinoPacket store(char&ast; key, float value, char&ast timestamp)*
##### *ArancinoPacket store(char&ast; key, long value, char&ast timestamp)*
##### *ArancinoPacket store(char&ast; key, uint32_t value, char&ast timestamp)*

Append (or create and append) a new sample to the series specified by key.

##### Parameters
* **`key`**: *key* to store.
* **`value`**: the *value* for the specified key.
* **`timestamp`**: the expicit *timestamp* for the specified key.

##### Return value
char&ast; reply: the insertion timestamp.

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING`;
  * `response.string`: `char*` pointer that contain the insertion timestamp to the series.
___
### mstore
##### *ArancinoPacket mstore(char&ast;&ast; key, char&ast;&ast; value, int len)*
##### *Void mstore(char&ast;&ast; key, char&ast;&ast; value, int len,char&ast timestamp)*
Append new samples to a list of series.

##### Parameters
* **`keys`**: the list of *keys* to store.
* **`values`**: corresponding values of the keys;
* **`len`**: number of keys to set, namely the length of keys array.
* **`timestamp`**: the expicit *timestamp* for the specified set of keys.

##### Return value
char&ast;&ast; reply: list of insertion timestamps.
Returns nothing (Void) if timestamp prototype is used. (it uses ExecuteCommandFast so it don't require a response from Arancino Daemon).

or

ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `STRING_ARRAY`;
  * `response.stringArray`: `char**` pointer that points to the start of the returned array of insertion timestamps to the series.
___
### storetags
##### *ArancinoPacket storetags(char&ast; key, char&ast;&ast; tags, char&ast;&ast; value, int len)*

##### Parameters
* **`key`**: the *key* to store.
* **`tags`**: the list of the *tags* to store.
* **`values`**: corresponding values of the *tags*;
* **`len`**: number of tags to set, namely the length of keys array.

##### Return value
ArancinoPacket reply: [ArancinoPacket](#arancinopacket) containing:
  * `isError`: API call outcome (`true` or `false`);
  * `responseCode`: the [response code](#variables) value.
  * `responseType`: `VOID`;
  * `response`: `NULL`;
___
### print
##### *void print(char&ast; message)*
Set the *message* as a *value* for the reserved *key* `___MONITOR___`. To do that it uses the `set` api under the hood.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  SERIAL_DEBUG.begin(115200);

}

void loop() {

  Arancino.print(0,"Hello from Arancino");
  delay(5000); //wait 5 seconds

}
```


___
### println
##### *void println(char&ast; message)*
Like [`print`](#print) but with a trailing *new line* char.

___
### getArraySize
##### *int getArraySize(char&ast;&ast; array)*
Return the *array* string count.
##### Parameters
* **`array`**: pointer to string array.

##### Return value
int reply: string count.

##### Example
```c++
#include <Arancino.h>

void setup() {
  SERIAL_DEBUG.begin(115200);
  Arancino.begin();

  Arancino.set("exgetarraysize_pressure",1023);
  Arancino.set("exgetarraysize_humidity",67.5);
  Arancino.set("exgetarraysize_temperature",24.4);

}

void loop() {
  char** key = Arancino.keys();
  int count = Arancino.getArraySize(key);
  SERIAL_DEBUG.print("Key count: ");
  SERIAL_DEBUG.println(count);
  Arancino.free(key);
}
```
___
### free
##### *void free(char&ast; string)*
##### *void free(char&ast;&ast; stringArray)*
##### *void free(ArancinoPacket packet)*

frees the memory used by a `char*` string, `char**` array string, or by an ArancinoPacket.
##### Parameters
* **`string`**: pointer to string.
* **`stringArray`**: pointer to string array.
* **`packet`**: ArancinoPacket variable.


##### Example
```c++
#include <Arancino.h>

void setup() {
  SERIAL_DEBUG.begin(115200);
  Arancino.begin();

  Arancino.set("exfree_foo", "bar");
  Arancino.set("exfree_qwe", "asd");
  Arancino.set("exfree_pressure",1023);
  Arancino.set("exfree_humidity",67.5);
  Arancino.set("exfree_temperature",24.4);

}

void loop() {
  char* str = Arancino.get("exfree_foo");
  char** key = Arancino.keys();
  ArancinoPacket myPacket = Arancino.getPacket("exfree_ qwe");

  /* user code... */

  Arancino.free(str);
  Arancino.free(key);
  Arancino.free(myPacket);
}
```
###### Note: when the *free* API is used for freeing an ArancinoPacket, the response type (string or string array ) is auto detected.
___
### isValidUTF8
##### * bool isValidUTF8(char&ast; string) *


Check if the string is valid and correct for the utf-8 standard.

##### Parameters
* **`string`**: pointer to string.

##### Example
```c++
#include <Arancino.h>

void setup() {
  SERIAL_DEBUG.begin(115200);
  Arancino.begin();
}

void loop() {
  char* str = "Test";
  if(isValidUTF8(str))
        Arancino.set("key3", str);

}
```
### Notes
1. When using `get`-type functions (such as `get`, `hget`, `mget`, `hkeys`, `hvals`, `hgetall`, ...) the Arancino Module first searches the key in the volatile redis db. If it is not found there, the persistent redis db is used.
2. When using `set` and `hset` functions the key has to be unique between both volatile and persistent db. Example: if we want to set a volatile variable named `key1`, the Arancino Module first checks if `key1` exists in the persistent db.
3. When using `mset` function the previous check is not executed. Later, when using `get` function the default redis db is first used (volatile).
4. When using arrays returned by Arancino Library functions it is important to free them after usage to prevent memory leaks. Example:
      ```c++
      char* temp = Arancino.get("test");
      Arancino.set("key2", temp);
      Arancino.free(temp);
      ```

      It is not recommended to do as follows:
      ```c++
      Arancino.set("key2", Arancino.get("test"));
      ```

5. When using `set`-type functions, make sure that the value is correctly formatted with *utf-8* standard. Is recommended to use the internal function `isValidUTF8` before the `set` to avoid decoding errors on Arancino Module. Example:
      ```c++
      char* value="test";
      if(isValidUTF8(value))
        Arancino.set("key3", value);

      ```

___
### getTimestamp
##### * char* getTimestamp() *


Return timestamp in UNIX millis format.

##### Return value
char&ast; timestamp: current timestamp.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
}

void loop() {
  char* ts = Arancino.getTimestamp();
  Arancino.set("ts", ts);
  delay(2000);
}
```
___
### getModuleVersion
##### * char* getModuleVersion() *


Return Arancino Module version.

##### Return value
char&ast; modVersion: current Module version.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
}

void loop() {
  char* modVers = Arancino.getModuleVersion();
  Arancino.set("mod_version", modVers);
  Arancino.free(modVers);
  delay(2000);
}
```
___
### getModuleLogLevel
##### * char* getModuleLogLevel() *


Return current Arancino Module log level.

##### Return value
char&ast; modVersion: current Module log level.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
}

void loop() {
  char* logLvl = Arancino.getModuleLogLevel();
  Arancino.set("Log_level", logLvl);
  Arancino.free(logLvl);
  delay(2000);
}
```
