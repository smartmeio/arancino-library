# Arancino Library
Arancino Library works on Arancino boards, it's written in Arduino lang and can be imported in Arduino IDE. Arancino Library it's written to runs on SAMD21 Microcontroller over SAMD Arduino platform. Arancino Library uses the serial connection to communicate with Arancino Module which runs in the linux side of the Arancino board. It uses `SerialUSB` for communication with Arancino Module and `Serial` for debug.

Arancino Library allows to export/import data to/from the Linux enviroment using Redis as database cache. Arancino Library API are modeled on Redis Commands.


## Getting Started
To start using Arancino Library, download the latest version from the repository `arancino-library` within the [SmartMe.IO Repository Management Site](https://download.smartme.io).

### Arduino IDE
Unzip _Arancino Library_ and open Arduino IDE and go to *Sketch* → *Include Library* → *Add .zip Library...* then open the just unzipped folder. *Arancino Library* will appears in *Sketch* → *Include Library* menu at the end, under the *Contributed* section. Examples will be under *File* → *Examples* → *Examples of Custom Libraries*.


## Arancino Library API

### begin
##### *void begin(int timeout)* 
##### *void begin()* 
Start the communication with Arancino Module.

##### Parameters
* **`timeout`**: rappresents the time each commands sent (via api call) will wait for a response. Otherwise a command will skipped. When `begin` is called w/o `timeout` it assume its default value: `100ms`.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
}

void loop() {
    //do something
}
```



___
### set
##### *void set( String key, String value )*
##### *void set( String key,  int value )*
##### *void set( String key, float value )*

Set *key* to hold the string *value*. If *key* already holds a *value*, it is overwritten, regardless of its type.

##### Parameters
* **`key`**: the *key* name 
* **`value`**: the *value* for the specified *key*. can be String, int o float  

##### Example

```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
  Arancino.set("foo","bar");
}

void loop() {
    //do something
}

```



___
### get
##### *String get( String key )*

Get the *value* of *key*. If the *key* does not exist TODO:~~the special value nil is returned.~~ 

#### Parameters
* **`key`**: the name of the key from which retrieve the value

#### Return value
XXX TODO

##### Example
```c++
#include <Arancino.h>

String value = "";

void setup() {
  Arancino.begin();
  Serial.begin(115200);
}

void loop() {
    Arancino.set("foo","bar");
    value = Arancino.get("foo");
    Serial.println("foo -> " value);
    //foo -> bar
    
    delay(2000);

    Arancino.set("foo","baz");
    value = Arancino.get("foo");
    Serial.println("foo -> " value);
    //foo -> baz
}

```


___
### del
##### *int del( String key )*

Removes the specified *key*. A *key* is ignored if it does not exist.

##### Parameters
* **`key`**: the name of the key to delete. 

##### Return value
int reply: The number of keys that were removed.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Serial.begin(115200);
  Arancino.begin();
  Arancino.set("foo","bar");

  int num = Arancino.del("baz");
  //0
  num = Arancino.del("foo");
  //1
}

void loop() {
  //do something
}

```



___
### keys
##### *String\* keys(String pattern)*
Returns all keys matching *pattern*.

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
String* reply: ~list~ of keys matching *pattern*.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Serial.begin(115200);
  Arancino.begin();

  Arancino.set("pressure",1023);
  Arancino.set("humidity",67.5);
  Arancino.set("temperature",24.4);

  String* key= Arancino.keys();
  
  for(int i=0;i<Arancino.arraySize;i++){
    if(i!=0)
      Serial.println(keys[i]);  
  }
  //pressure
  //humidity
  //temperature


}

void loop() {
  //do something
}

```



___
### hset
##### *int hset( String key, String field , String value )*
##### *int hset( String key, String field , int value )*
##### *int hset( String key, String field , float value )*

Sets *field* in the hash stored at *key* to *value*. If *key* does not exist, a new *key* holding a hash is created. If *field* already exists in the hash, it is overwritten.

##### Parameters
* **`key`**: the name of the *key* used to create the hash.
* **`field`**: the name of the *field* to store in the hash.
* **`value`**: the value to store in the hash with the specified field.

##### Return value
int reply:
- 1 if *field* is a new field in the hash and *value* was set.
- 0 if *field* already exists in the hash and the *value* was updated.

##### Example
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();

  int resp = Arancino.hset("foo","bar","yeah");
  //1
  resp = Arancino.hset("foo","baz","whoo");
  //0
}

void loop() {
  //do something
}

```



___
### hget
##### *String hget( String key, String field ) {*
Returns the *value* associated with *field* in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.
* **`field`**: the name of the *field* from which retrieve the value

##### Return value
String reply: TODO (check if is correct)
- the *value* if a value is stored in *field* at *key*
- NULL if there's not value stored 

##### Example
TODO



___
### hgetall
##### *String\* hgetall( String key )*
Returns all fields and values of the hash stored at *key*. In the returned value, every *field* name is followed by its *value*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### hkeys
##### *String\* hkeys( String key )*
Returns all field names in the hash stored at key.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### hvals
##### *String\* hvals( String key )*
Returns all values in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### hdel
##### *int hdel( String key, String field )*
Removes the specified *field* from the hash stored at *key*. ~If specified *field* that does not exist within this hash, this commands returns 0~. If key does not exist, it is treated as an empty hash and this command returns 0.
TODO check if correct



##### Parameters
* **`key`**: the name of the *key* which hold the hash.
* **`field`**: the name of the *field* to delete.

##### Return value
int reply: the number of fields that were removed from the hash.

##### Example
TODO



## Cortex Protocol
Arancino Library uses a simple protocol, called **Cortex**, to communicate with Arancino Module over serial connection. Cortex Protocol is designed to be easy to read and processed. Arancino Library, Arancino Module and Cortex Protocol are designed to be monodirectional and synchronous. In this scenario Arancino Library within the microcontoller acts as *master*, and Arancino Module as *slave*.


Each command sent using Cortex Protocol is composed by a *command identifier* and one or more *parameters*. *Command identifiers* and *parameters* are separated by a separator char, and the command ends with and end chars. Each sent commands will receive a response, formed by a Response Code and one or more retuerned values. Each items are separated by the same separtor char and ends with the same end char. Commands receive Response within an timeout interval of `100ms`. If no Response are received, Command will be skipped.

### Commands identifier:


| API               | Command Identifiers    |
| ------------------ |:-------------:|
| [`begin`](#begin)  | SET           |
| [`set`](#set)      | SET           |
| [`get`](#get)      | GET           |
| [`del`](#del)      | DEL           |
| [`keys`](#keys)    | KEYS          |
| [`hget`](#hget)    | HGET          |
| [`hset`](#hset)    | HSET          |
| [`hkeys`](#hkeys)  | HKEYS         |
| [`hvals`](#hvals)  | HVALS         |
| [`hdel`](#hdel)    | HDEL          |

### Commands separator chars
**Important**: Do not use these char codes to compose String values to pass to the API

| Separator             | Char Code     |
| --------------------- |:-------------:|
| Command Sepatator     | `4`           |
| End of transmission   | `30`          |


### Response Codes

| Response Code     | Description           |
| ----------------- |:---------------------:|
| `100`             | **OK** - Generic operation successfully completed. |
| `101`             | **OK** - Setted value into a new field.            |
| `102`             | **OK** - Setted value into an nexistingew field    |
| `200`             | **KO** - Generic Error                             |
| `201`             | **KO** - Retrieved NULL value                      |
| `202`             | **KO** - Error during *SET* command                |
| `203`             | **KO** - Command not found                         |
| `204`             | **KO** - Command not received                      |
| `205`             | **KO** - Invalid parameter number                  |
| `206`             | **KO** - Generic Redis Error                       |

### Commands and Protocol
As exaplained above, when make a call to api function, a command is sent over the `SerialUSB` and a response is recevied:
In the next paragraphs, for semplicity we are considering each command returns OK response and even the following rappresentation for *Separator Codes*:
- Command Sepatator → `4`  → `#`
- End of transmission → `30`  →` @`


#### begin
- Command Sent: `START@
- Response Received: `100@`

#### set
- Command Sent: `SET#<key>#<value>@`
- Response Received: `100@`

#### get
- Command Sent: `GET#<key>@`
- Response Received: `100#<value>@`

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
- Command Sent: `HGETALL#<key>#<field>@`
- Response Received: `100[#<field-1>#<value-1>#<field-2>#<value-2>]@`

#### hkeys
- Command Sent: `HKEYS#<key>@`
- Response Received: `100HKEYS#<key>@`

#### hvals
- Command Sent: `HVALS#<key>@`
- Response Received: `100[#<value-1>#<value-2>]@`

#### hdel
- Command Sent: `HDEL#<key>#<field>@`
- Response Received: `100#1@`

#### KO Reponse common to all api function:
- `200@`
- `205@`
- `206@`


## Debug 
It's a feature introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1). Debug mode can be enabled in Arancino Board by setting the `GPIO PA31` - Pin number `26`; after setting the Debug GPIO, power on Arancino Board (or resetting the microtroller if previously powered on) Debug data will be sent over the 'Serial` and you can read it trough a serial console, like cu, screen or the Arduino IDE Serial Monitor.


### Responde Code
Each command sent via the API, will receive a response. Response contains the _Response Code_ and one or more _Response Value_. See [Commands and Protocol](#Commands and Protocol)

### Debug Messages
Debug messages are similar to those written above in the [Commands and Protocol](#Commands and Protocol) paragraph, but consider that the _Separator Chars_ are no-printable so a message will appears in a serial console like the following:

- Command Sent: `GETkey`
- Response Received:`100value`


## Credits
Much of the documentation has been extrapolated from [Redis Command](https://redis.io/commands/)
Redis and Arduino are trademarks of their respective owners.