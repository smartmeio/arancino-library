# Arancino Library
The following Library works on Arancino boards, it is written in Arduino lang and can be imported into the Arduino IDE. Arancino Library is written to run on SAMD21 Microcontroller over SAMD Arduino platform. It uses the serial connection to communicate with the Arancino Module which runs on the linux side of the Arancino board. It uses the `SerialUSB` library to communicate with the Arancino Module and the `Serial` library for debug.

Arancino Library allows to export/import data to/from the Linux environment using Redis as database cache. The API are modelled on Redis standard Commands.

## **Secure Authentication and Communication**
Secure authentication between *peripheral units* and a *Main* allows to start a connection with the *Main* only to *peripheral units* previously choice , to achieve this is used the ECDSA (Elliptic Curve Digital Signature Algorithm). The *Main* receive device and signer certificates with the `START` command, verify the certificates validity and the presence of device public key in a whitelist, finally send back the response of the `START` command with a *challenge* (a random number codified with base64 encryption) in the configuration. When the *peripheral unit* receive the response with the *challenge* sign it with its private key and send it back in the `SIGN` command configuration (the *signature*), codified with base64 encryption. The *Main* receive the `SIGN` command and verify the correctness of the *signature*, then if it's verified the *peripheral unit* is authenticated and a *challenge* is send back in the response configuration.

Secure communication is implemented with the help of the *challenge* present in the previous response configuration, every time that a command is sent from the *peripheral unit* it has to contain the *signature* of the *challenge* received with the previous *response*, starting with the *challenge* received in the `SIGN` *response*.

The secure authentication and communication is used only when the flag `SCR_MOD` in the `START` response is 1

## Getting Started
To get started with Arancino Library you can download the latest version from the repository `arancino-library` within the [smartme.IO Repository Management Site](https://download.smartme.io/artifactory/list/arancino-library/) or directly from Arduino Library Manager.

### Arduino IDE

#### from a zip file
Open the Arduino IDE, unzip the _Arancino Library_ and import the unzipped folder (*Sketch* → *Include Library* → *Add .zip Library...*). The library will appear under *Sketch* → *Include Library* menu at the end, under the *Contributed* section. Examples will be under *File* → *Examples* → *Examples of Custom Libraries*.

#### from Arduino Library Manager
Open the Arduino IDE and go to *Sketch* → *Include Library* → *Manage Libraries*, the Arduino Library Manager window will be shown. Using the text box, type _Arancino_; finally selecte the Arancino Library item within the result list and click install. Be sure to select the latest version available.x

*Important Note:*
> Arancino Library has one dependency when FreeRTOS capabilities are required: [FreeRTOS_SAMD21 Arduino library](https://github.com/BriscoeTech/Arduino-FreeRTOS-SAMD21) by BriscoeTech. Please download it from Arduino Library Manager. In that case you also have to select _Yes_ from _Menu -> Tools -> Using FreeRTOS?_.

## Testing the Library
It is possible to run unit tests for the Arancino Library. In order to do so you need to:

1. Flash `test/unit.ino` sketch on the microcontroller.
2. Run `test/unit.sh` on the host where Arancino Module is installed. *
3. Output is written to `Serial1`.

*: It is possible to modify connection variables relative to redis databases inside `unit.sh` file.

## Data structures
### ArancinoMetadata
`ArancinoMetadata` is a data structure used to encapsulate firmware metadata.

#### Content
```c++
typedef struct {
	char* fwname;
	char* fwversion;
	char* tzoffset;
} ArancinoMetadata;
```

#### Variables
* `fwname`: represents the firmware name, it is later read and displayed by Arancino Module;
* `fwversion`: represents the firmware version;
* `tzoffset`: is the local timezone offset. It is used to calculate the timestamp at which the firmware is compiled;

### ArancinoPacket
`ArancinoPacket` is a data structure returned from some Arancino APIs and contain details about the APIs calling result.

##### Content
```c++
typedef struct {
  bool isError;
  int responseCode;
  int responseType;
  ArancinoResponse response;
} ArancinoPacket;
```

### ArancinoConfig
`ArancinoConfig` is class composed only by attributes used to configure the Arancino Library behaviour. It replaces the use of direct configuration arguments of the `begin` function.
```c++
    int _TIMEOUT = 100 ;  // *DEPRECATED* Use SERIAL_TIMEOUT
    bool _USEID;          // *DEPRECATED* Use USE_PORT_ID_PREFIX_KEY
    int DECIMAL_DIGITS = 4; 
    int SERIAL_TIMEOUT = 100;
    bool USE_PORT_ID_PREFIX_KEY = false;

```

- **`SERIAL_TIMEOUT`**:  Represents the time that each command sent (via API call) will wait
            for a response. Otherwise the delayed command will be skipped. When the
            `begin` function is called w/o `timeout` it is assumed `100ms` as
            default value of timeout.
- **`USE_PORT_ID_PREFIX_KEY`**: Default `false`. If `true` each key setted up is prefixed with ID of the microntroller.
        It's useful when you connect multiple microntroller with the same firmware (using the same keys) to
        one Arancino Module; By this way, at the application level, you can distinguish keys by
        microntroller id.
- **`DECIMAL_DIGITS`**: Default `4`. Represents the number to the right of the decimal point for the float and double data types.
				Float data type has 7 digits, while double data type has up to 15 digits, including decimal digits.
        If the digits exceed the maximum allowed for the type(float or double), the decimal part will be truncated and rounded up or rounded down.

##### Variables
* `isError`: indicates the outcome of an API call. Its value is `False` (0) when everything is OK or `True` (1) when an error occurs. Both in positive and negative cases is possible to check `responseCode` for more details;
* `responseCode`: contains a code relative to API call outcome, identified by a label as follows (codes from `100` are defined by the [Cortex Protocol](#response-codes):
    * `#define RESERVED_KEY_ERROR      -3`: returned when trying to set a reserved key;
    * `#define COMMUNICATION_ERROR     -2`: returned when an UART communication error occur;
    * `#define GENERIC_ERROR           -1`: returned on generic and not specific error;
    * `#define RSP_OK					        100`: Redis OK - Generic operation successfully completed;
    * `#define RSP_HSET_NEW			      101`: Redis OK - Setted value into a new field;
    * `#define RSP_HSET_UPD		       	102`: Redis OK - Setted value into an existing field;
    * `#define ERR						        200`: Redis KO - Generic Error;
    * `#define ERR_NULL				        201`: Redis KO - Retrieved NULL value;
    * `#define ERR_SET					      202`: Redis KO - Error during SET command;
    * `#define ERR_CMD_NOT_FND		    203`: Redis KO - Command not found;
    * `#define ERR_CMD_NOT_RCV		    204`: Redis KO - Command not received;
    * `#define ERR_CMD_PRM_NUM		    205`: Redis KO - Invalid parameter number;
    * `#define ERR_REDIS				      206`: Redis KO - Generic Redis Error;
    * `#define ERR_REDIS				      207`: Redis KO - Key exists in the Standard Data Store;
    * `#define ERR_REDIS				      208`: Redis KO - Key exists in the Persistent Data Store;
    * `#define ERR_REDIS				      209`: Redis KO - Non compatibility between Arancino Module and Library;
* `responseType`: contains a code that indicate the type of the reponse, identified as follows:
    * `#define VOID            0`: returned when the API call does not provide any data, e.g. `set` command (see `responseCode` to evaluate the outcome);
    * `#define INT             1`: returned when the Redis response is an integer value, e.g. `del` command;
    * `#define STRING          2`: returned when the Redis response is a string, e.g. `get` command;
    * `#define STRING_ARRAY    3`: returned when the Redis response is an array string, e.g. `keys` command.
* `response`: is a data structure that contain the Redis response. Its content can be an `int`, `char*` or `char**`, for example:
    * `response.integer`: interprets the content as an `int` type;
    * `response.string`: interprets the content as an `char*` type;
    * `response.stringArray`: interprets the content as an `char**` type;

> **Note**: when the `ArancinoPacket` returned from an API contain a string or an array string, the user must manually free the ArancinoPacket (or directly the `char*` or `char**` associated pointer) using the [`Arancino.free()`](#free) API; this is necessary to avoid memory leaks.


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
### print
##### *void print(char&ast; message)*
Set the *message* as a *value* for the reserved *key* `___MONITOR___`. To do that it uses the `set` api under the hood.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);

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
  Serial.begin(115200);
  Arancino.begin();

  Arancino.set("exgetarraysize_pressure",1023);
  Arancino.set("exgetarraysize_humidity",67.5);
  Arancino.set("exgetarraysize_temperature",24.4);

}

void loop() {
  char** key = Arancino.keys();
  int count = Arancino.getArraySize(key);
  Serial.print("Key count: ");
  Serial.println(count);
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
  Serial.begin(115200);
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
  Serial.begin(115200);
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

## Cortex Protocol
Arancino Library uses a simple protocol, called **Cortex**, to communicate with the Arancino Module over serial connection. Cortex Protocol is designed to be easy to read and processed. Arancino Library, Arancino Module and Cortex Protocol are designed to be monodirectional and synchronous. In this scenario the Arancino Library within the microcontroller acts as *master*, and the Arancino Module as *slave*.


Each command sent using Cortex Protocol is composed by a *command identifier* and one or more *parameters*. *Command identifiers* and *parameters* are separated by a separator character, and the command ends with an end character. Each command sent will receive a response, formed by a Response Code and one or more returned values. All the items are separated by the same separtor character and ends with the same end character. The commands receive Response within a timeout interval of `100ms`. If no Response is received, the Command will be skipped.

### Commands identifier:


| API               | Command Identifiers    |
| ------------------ |:-------------:|
| [`begin`](#begin)  | START         |
| [`set`](#set)      | SET           |
| [`get`](#get)      | GET           |
| [`mset`](#mset)    | MSET          |
| [`mget`](#mget)    | MGET          |
| [`del`](#del)      | DEL           |
| [`keys`](#keys)    | KEYS          |
| [`hget`](#hget)    | HGET          |
| [`hset`](#hset)    | HSET          |
| [`hkeys`](#hkeys)  | HKEYS         |
| [`hvals`](#hvals)  | HVALS         |
| [`hdel`](#hdel)    | HDEL          |
| [`flush`](#flush)  | FLUSH         |
| [`publish`](#publish)    | PUB     |
| [`mset`](#mset)    | MSET     |
| [`mget`](#mget)    | MGET     |

### Commands separator chars
**Important**: Do not use these character codes to compose string values to pass to the API

| Separator             | Char Code     |
| --------------------- |:-------------:|
| Command Sepatator     | `4`           |
| Array separator       | `16`          |
| NULL character        | `25`          |
| End of transmission   | `30`          |


### Response Codes

| Response Code     | Description           |
| ----------------- |:---------------------:|
| `100`             | **OK** - Generic operation successfully completed. |
| `101`             | **OK** - Setted value into a new field.            |
| `102`             | **OK** - Setted value into an existing field       |
| `200`             | **KO** - Generic Error                             |
| `201`             | **KO** - Retrieved NULL value (deprecated)         |
| `202`             | **KO** - Error during *SET* command                |
| `203`             | **KO** - Command not found                         |
| `204`             | **KO** - Command not received                      |
| `205`             | **KO** - Invalid parameter number                  |
| `206`             | **KO** - Generic Redis Error                       |
| `207`             | **KO** - Key exists in the Standard Data Store     |
| `208`             | **KO** - Key exists in the Persistent Data Store   |
| `209`             | **KO** - Non compatibility between Arancino Module and Library |

### Commands and Protocol
As exaplained above, when an API function is called, a command is sent over the `SerialUSB` and a response is received.
In the next paragraphs, for simplicity we are considering each command returns an *OK* response and using the following representation for *Separator Codes*:
- Command Sepatator → `30`  → `#`
- Array separator → `16` → `%`
- End of transmission → `4`  →` @`

WITHOUT SECURE MODE

#### begin
- Command Sent: `START#<lib vers>#<fwname>#<fwversion>#<compiletime>#<coreversion>@`
- Response Received: `100#<arancino-id>#<timestamp>@`

#### set
- Command Sent: `SET#<key>#<value>@`
- Response Received: `100@`

#### get
- Command Sent: `GET#<key>@`
- Response Received: `100#<value>@`

#### mset
- Command Sent: `MSET#<key-1>%<key-2>...%<key-n>#<val-1>%<val-2>...%<val-n>@`
- Response Received: `100#@`

#### mget
- Command Sent: `MGET#<key-1>%<key-2>...%<key-n>@`
- Response Received: `100#<val-1>#<val-2>...#<val-n>]@`

#### del
- Command Sent: `DEL#<key>@`
- Response Received: `100#1@`

#### keys
- Command Sent: `KEYS#<pattern>@`
- Response Received: `100[#<key-1>#<key-2>#<key-n>]@`

#### hset
- Command Sent: `HSET#<key>#<field>#<value>@`
- Response Received: 101@

#### hget
- Command Sent: `HGET#<key>#<field>@`
- Response Received: `100#<value>@`

#### hgetall
- Command Sent: `HGETALL#<key>@`
- Response Received: `100#<field-1>#<value-1>#<field-2>#<value-2>@`

#### hkeys
- Command Sent: `HKEYS#<key>@`
- Response Received: `100HKEYS#<key>@`

#### hvals
- Command Sent: `HVALS#<key>@`
- Response Received: `100#<value-1>#<value-2>@`

#### hdel
- Command Sent: `HDEL#<key>#<field>@`
- Response Received: `100#1@`

#### flush
- Command Sent: `FLUSH@`
- Response Received: `100@`

WITH SECURE MODE

#### begin
- Command Sent: `START#<lib vers>#<fwname>#<fwversion>#<compiletime>#<coreversion>#<devicecertificate>#<signercertificate>@`
- Response Received: `100#<arancino-id>#<timestamp>#<challenge>@`

#### sign
- Command Sent: `SIGN#<signature>@`
- Response Received: `100#<challenge>@`

#### set
- Command Sent: `SET#<key>#<value>#<signature>@`
- Response Received: `100#<challenge>@`

#### get
- Command Sent: `GET#<key>#<signature>@`
- Response Received: `100#<value>#<challenge>@`

#### mset
- Command Sent: `MSET#<key-1>%<key-2>...%<key-n>#<val-1>%<val-2>...%<val-n>#<signature>@`
- Response Received: `100#<challenge>@`

#### mget
- Command Sent: `MGET#<key-1>%<key-2>...%<key-n>#<signature>@`
- Response Received: `100#<val-1>#<val-2>...#<val-n>]#<challenge>@`

#### del
- Command Sent: `DEL#<key>#<signature>@`
- Response Received: `100#1#<challenge>@`

#### keys
- Command Sent: `KEYS#<pattern>#<signature>@`
- Response Received: `100[#<key-1>#<key-2>#<key-n>]#<challenge>@`

#### hset
- Command Sent: `HSET#<key>#<field>#<value>#<signature>@`
- Response Received: 101#<challenge>@

#### hget
- Command Sent: `HGET#<key>#<field>#<signature>@`
- Response Received: `100#<value>#<challenge>@`

#### hgetall
- Command Sent: `HGETALL#<key>#<signature>@`
- Response Received: `100#<field-1>#<value-1>#<field-2>#<value-2>#<challenge>@`

#### hkeys
- Command Sent: `HKEYS#<key>#<signature>@`
- Response Received: `100HKEYS#<key>#<challenge>@`

#### hvals
- Command Sent: `HVALS#<key>#<signature>@`
- Response Received: `100#<value-1>#<value-2>#<challenge>@`

#### hdel
- Command Sent: `HDEL#<key>#<field>#<signature>@`
- Response Received: `100#1#<challenge>@`

#### flush
- Command Sent: `FLUSH#<signature>@`
- Response Received: `100#<challenge>@`

#### publish
- Command Sent: 'PUBLISH#<channel>#<message>#<signature>@'
-Response Received: '100#1#<challenge>@'

## Reserved Keys

**PAY ATTENTION: NOT EVERYTHING DESCRIBED BELLOW IN THIS PARAGRAPH IS YET AVAILABLE**

Arancino Library uses some reserved keys to store environmental data into Redis. Values stored in those keys can be used for multiple scopes in user space. By default the Arancino Library uses those reserved keys to communicate in a synchronous way but it can do also in an asynchronous way.

Following the list of reserved keys:

| Reserved Key/Channel    | Version | Description    |
| ----------------------- | ------- | -------------- |
| `___POWER___`           | N/A     | The value can be `battery` or `power`, based on the power supply type. Very useful e.g. when switching from power to battery and automatically softly shoutdown the system. It works only with the specific hardware module for the Arancino Board. (Not yet implemented)|
| `___MONITOR___`         | >=v0.1.0| used by `print` and `println` API |
| `__LIBVERS___`          | >=v0.1.0| Arancino Library version (No more used) |


To access data stored at reserved keys you have to use the Redis `get` command for _Sync_ mode and Redis `Subscribe` for Asynch mode (in _Async_ mode the _key_ name represents the _channel_ name).

By default the Arancino Library uses the _Sync_ mode, but you can change it to work in _Async_ mode or _Both_ (_Sync_ and _Async_ at the same time). To do that you have to define a variable called `RSVD_COMM` at the beginning of your program, with three possible values:

- `0` →  _Sync_
- `1` →  _Async_
- `2` →  _Both_

Example:
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
  Arancino.setReservedCommunicationMode(BOTH);
}

void loop() {
  //do something
}

```


## Debug
This feature was introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1) and can be enabled in the Arancino Board by setting the `GPIO PA31` - Pin number `26`; after setting the Debug GPIO, power on the Arancino Board (or resetting the microtroller if previously powered on), the Debug data will be sent over the 'Serial` and you can read it through a serial console, like cu, screen or the Arduino IDE Serial Monitor.


### Responde Code
Each command sent via the API, will receive a response. Response contains the _Response Code_ and one or more _Response Value_. See [Commands and Protocol](#Commands and Protocol)

### Debug Messages
Debug messages are similar to those written above in the [Commands and Protocol](#Commands and Protocol) paragraph, but consider that the _Separator Chars_ are no-printable so a message will appears in a serial console like the following:

- Command Sent: `GETkey`
- Response Received:`100value`

## FreeRTOS implementation
The following section concern the use of FreeRTOS, already implemented in the Arancino Library; will be illustrated the Arancino-related FreeRTOS APIs and some basic FreeRTOS functions. For more advanced features, please see the official [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html).

### FreeRTOS library
The Arancino library use a [FreeRTOS porting](https://github.com/BriscoeTech/Arduino-FreeRTOS-SAMD21) for SAMD architecture that can be installed directly from the official Arduino library repository through the library manager. __User must install the correct FreeRTOS library__; the correct function it's not guaranteed with other FreeRTOS libraries. User doesn't have to include the FreeRTOS library because it is already included in Arancino library.

___
### startScheduler
##### *void startScheduler*
Configure the `LED_BUILTIN` for debug and start the FreeRTOS scheduler. When a fatal FreeRTOS error occur, the `LED_BUILTIN` will blink following this codes:

-    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
-    2 blinks  - Stack overflow, task needs more bytes defined for its stack.
-    1 blink - Malloc failed, probably ran out of heap.

> **IMPORTANT**: `Arancino.startScheduler()` never return, so **MUST be** the last instruction of Arduino begin() function.

##### Example:
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
  Arancino.startScheduler();
}

void loop() {
    //do something
}
```

### Basic FreeRTOS APIs
___
### xTaskCreate
##### BaseType_t xTaskCreate(TaskFunction_t pvTaskCode, const char &ast;pcName, configSTACK_DEPTH_TYPE usStackDepth, void &ast;pvParameters, UBaseType_t uxPriority, TaskHandle_t &ast; pxCreatedTask);

Create a new task and add it to the list of tasks that are ready to run.

##### Parameters
* **`pvTaskCode`**: Pointer to the task entry function (just the name of the function that implements the task, see the example below).
Tasks are normally implemented as an infinite loop, and must never attempt to return or exit from their implementing function. Tasks can however [delete themselves](https://www.freertos.org/a00126.html).

* **`pcName`**:	A descriptive name for the task. This is mainly used to facilitate debugging, but can also be used to [obtain a task handle](https://www.freertos.org/a00021.html#xTaskGetHandle).
The maximum length of a task’s name is set using the config `MAX_TASK_NAME_LEN` parameter in `FreeRTOSConfig.h`.

* **`usStackDepth`**:	The number of words (not bytes!) to allocate for use as the task’s stack. For example, if the stack is 16-bits wide and _usStackDepth_ is 100, then 200 bytes will be allocated for use as the task’s stack. As another example, if the stack is 32-bits wide and _usStackDepth_ is 400 then 1600 bytes will be allocated for use as the task’s stack.
The stack depth multiplied by the stack width must not exceed the maximum value that can be contained in a variable of type `size_t`. See the FAQ [How big should the stack be?](https://www.freertos.org/FAQMem.html#StackSize).

* **`pvParameters`**:	A value that will passed into the created task as the task’s parameter.
If _pvParameters_ is set to the address of a variable then the variable must still exist when the created task executes – so it is not valid to pass the address of a stack variable.

* **`uxPriority`**:	The [priority](https://www.freertos.org/RTOS-task-priority.html) at which the created task will execute.

* **`pxCreatedTask`**: Used to pass a [handle](https://www.freertos.org/a00019.html#xTaskHandle) to the created task out of the xTaskCreate() function. pxCreatedTask is optional and can be set to NULL.

##### Return value
BaseType_t return: If the task was created successfully then `pdPASS` is returned. Otherwise `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` is returned.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    Arancino.set("foo", "bar");
    vTaskDelay(2000); //wait 2 seconds (non-blocking delay)
  }
}

void setup() {
  Arancino.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
```
___
### vTaskDelay
##### void vTaskDelay(const TickType_t xTicksToDelay);
Delay a task for a given number of ticks. The actual time that the task remains blocked depends on the tick rate. The constant `portTICK_PERIOD_MS` can be used to calculate real time from the tick rate – with the resolution of one tick period.

##### Parameters
* **`xTicksToDelay`**: The amount of time, in tick periods, that the calling task should block. If, as in this implementation, the tick rate is 1000hz, the `portTICK_PERIOD_MS` is `1` so `xTicksToDelay` corresponds to milliseconds.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    vTaskDelay(1000); //wait 1 second (non-blocking delay)
  }
}

void setup() {
  Arancino.begin();
  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```
___
### vTaskSuspend
##### void vTaskSuspend(TaskHandle_t xTaskToSuspend);
Suspend any task. When suspended a task will never get any microcontroller processing time, no matter what its priority. Calls to vTaskSuspend are not accumulative – i.e. calling vTaskSuspend() twice on the same task still only requires one call to vTaskResume() to ready the suspended task.

##### Parameters
* **`xTaskToSuspend`**: Handle to the task being suspended. Passing a NULL handle will cause the calling task to be suspended.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;

void loop2(void *pvPramaters)
{
  while (1)
  {
    vTaskSuspend(loop2Handle); //executed only once - never resumed.
  }
}

void setup() {
  Arancino.begin();
  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```

___
### vTaskResume
##### void vTaskResume(TaskHandle_t xTaskToResume);
Resumes a suspended task. A task that has been suspended by one or more calls to vTaskSuspend () will be made available for running again by a single call to vTaskResume ().

##### Parameters
* **`xTaskToSuspend`**: Handle to the task being readied.

##### Example
```c++
#include <Arancino.h>
TaskHandle_t loop2Handle;
int state = 0;

void loop2(void *pvPramaters)
{
  while (1)
  {
    state = !state;
    digitalWrite(LED_BUILTIN, state); //blink
    vTaskSuspend(loop2Handle);
  }
}

void loop3(void *pvPramaters)
{
  while (1)
  {
    vTaskResume(loop2Handle);
    vTaskDelay(1000);
  }
}

void setup() {
  Arancino.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  xTaskCreate(loop2,     "Loop 2",       256, NULL, tskIDLE_PRIORITY, &loop2Handle);
  xTaskCreate(loop3,     "Loop 3",       256, NULL, tskIDLE_PRIORITY, NULL);
  Arancino.startScheduler();
}

void loop() {
  //do something
}
```

## Credits
Most of this documentation has been extrapolated from [Redis Command](https://redis.io/commands/), and [FreeRTOS quick start guide](https://www.freertos.org/FreeRTOS-quick-start-guide.html).

Redis, Arduino, Atmel and FreeRTOS are trademarks of their respective owners.
