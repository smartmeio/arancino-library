# Arancino Library
The following Library works on Arancino boards, it is written in Arduino lang and can be imported into the Arduino IDE. Arancino Library is written to run on SAMD21 Microcontroller over SAMD Arduino platform. It uses the serial connection to communicate with the Arancino Module which runs on the linux side of the Arancino board. It uses the `SerialUSB` library to communicate with the Arancino Module and the `Serial` library for debug.

Arancino Library allows to export/import data to/from the Linux environment using Redis as database cache. The API are modelled on Redis standard Commands.


## Getting Started
To start using Arancino Library, download the latest version from the repository `arancino-library` within the [SmartMe.IO Repository Management Site](https://download.smartme.io/artifactory/arancino-library/).

### Arduino IDE
Open the Arduino IDE, unzip the _Arancino Library_ and import the unzipped folder (*Sketch* → *Include Library* → *Add .zip Library...*). The library will appear under *Sketch* → *Include Library* menu at the end, under the *Contributed* section. Examples will be under *File* → *Examples* → *Examples of Custom Libraries*.


## API

### begin
##### *void begin(int timeout)*
##### *void begin()*
Start the communication with the Arancino Module.

##### Parameters
* **`timeout`**: represents the time that each command sent (via API call) will wait for a response. Otherwise the delayed command will be skipped. When the `begin` function is called w/o `timeout` it is assumed `100ms` as default value of timeout.

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

Get the *value* of *key*. If the *key* does not exist, the special value NULL is returned.

#### Parameters
* **`key`**: the name of the key from which the value is retrieved

#### Return value
String reply:
- the value of the passed *key*.
- NULL if the *key* doesn't exist.

##### Example
```c++
#include <Arancino.h>

String value = "";

void setup() {
  Arancino.begin();
  Serial.begin(115200);
}

void loop() {

  //sets the value 'bar' into the 'foo' key
  Arancino.set("foo","bar");

  //gets the value from the 'foo' key
  value = Arancino.get("foo");
  Serial.print("foo -> ");
  Serial.println(value);
  //foo -> bar

  delay(2000);

  Arancino.set("foo","baz");

  //gets the value from the 'foo' key
  value = Arancino.get("foo");
  Serial.print("foo -> ");
  Serial.println(value);
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

}

void loop() {

  String* key = Arancino.keys();
  int key_size=Arancino.getArraySize();
  for(int i=0;i<key_size;i++){
      Serial.println(key[i]);  
  }
	//pressure
	//humidity
	//temperature

  delay(5000); //wait 5 seconds
  
}

```



___
### hset
##### *int hset( String key, String field , String value )*
##### *int hset( String key, String field , int value )*
##### *int hset( String key, String field , float value )*

Sets *field* in the hash stored at *key* with the specified *value*. If *key* does not exist, a new *key* holding a hash is created. If *field* already exists in the hash, it is overwritten.

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
* **`field`**: the name of the *field* from which the value is retrieved

##### Return value
String reply:
- the *value* if a value is stored in *field* at *key*.
- NULL if there isn't a value stored.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  int resp = Arancino.hset("foo","bar","yeah");
  resp = Arancino.hset("foo","baz","whoo");

}

void loop() {
  
  String value = Arancino.hget("foo","bar");
  Serial.print("foo bar -> ");
  Serial.println(value);
  //foo bar -> yeah
  
  value = Arancino.hget("foo","baz");
  Serial.print("foo baz -> ");
  Serial.println(value);
  //foo bar -> whoo

  delay(5000); //wait 5 seconds

}
```

___
### hgetall
##### *String\* hgetall( String key )*
Returns all fields and values of the hash stored at *key*. In the returned value, every *field* name is followed by its *value*.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
*String reply: ~list~ of field and value matching *key*.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  
  Serial.begin(115200);
  
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");
  
}

void loop() {

  String* values = Arancino.hgetall("foo");
  for(int i=0; i<Arancino.getArraySize(); i+=2){
  	Serial.print("foo ");
  	Serial.print(values[i]);
  	Serial.print(" -> ");
  	Serial.println(values[i+1];
  	// foo bar -> yeah
  	// foo baz -> whoo
  }

  delay(5000); //wait 5 seconds
}
```


___
### hkeys
##### *String\* hkeys( String key )*
Returns all field names in the hash stored at key.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
*String reply: ~list~ of field matching *key*.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");

}

void loop() {

  String* fields = Arancino.hkeys("foo");
  for(int i=0; i<Arancino.getArraySize(); i++){
    Serial.print("foo -> ");
    Serial.println(fields[i]);
    // foo -> bar
    // foo -> baz
  }

  delay(5000); //wait 5 seconds
}
```


___
### hvals
##### *String\* hvals( String key )*
Returns all values in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which holds the hash.

##### Return value
*String reply: ~list~ of value matching *key*.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);
  
  Arancino.hset("foo","bar","yeah");
  Arancino.hset("foo","baz","whoo");

}

void loop() {
  
  String* values = Arancino.hkeys("foo");
  for(int i=0; i<Arancino.getArraySize(); i++){
    Serial.print("foo -> ");
    Serial.println(values[i]);
    // foo -> yeah
    // foo -> whoo
  }

  delay(5000); //wait 5 seconds
}


```



___
### hdel
##### *int hdel( String key, String field )*
Removes the specified *field* from the hash stored at *key*. If *field* is specified and it does not exist within this hash, this command returns 0. If the key does not exist, it is treated as an empty hash and this command returns 0.


##### Parameters
* **`key`**: the name of the *key* stored in the hash.
* **`field`**: the name of the *field* stored in the hash at key to delete.

##### Return value
int reply:
- 1 if the *field* is removed from hash.
- 0 if the *field* or the key does not exist in the hash.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  int resp = Arancino.hset("foo","bar","yeah");
  resp = Arancino.hset("foo","baz","whoo");
  int value = Arancino.hdel("foo","bar");
  //1
  value = Arancino.hget("foo","baz");
  //0

}

void loop() {
  //do something
}
```


___
### flush
##### *void flush()*
Delete all the keys.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Arancino.set("foo","bar");
  Arancino.set("foo","baz");
  //delete all the keys
  Arancino.flush();

}

void loop() {
  //do something
}
```
___
### publish
##### *int publish(String channel, String message)*
##### *int publish(int channel, String message)*
Posts a message to the given channel.


##### Parameters
* **`channel`**: the name of the *channel* where message will be send.
* **`message`**: *message* to send.

##### Return value
int reply: the number of clients that received the message.

##### Example
```c++
#include <Arancino.h>

void setup() {

  Arancino.begin();
  Serial.begin(115200);

}

void loop() {
  
  int resp = Arancino.publish(0,"hello from Arancino");
  Serial.print("message send to ")
  Serial.print(resp);
  Serial.println(" client");
  //message send to 0 client 
  
  delay(5000); //wait 5 seconds
  
}
```


## Cortex Protocol
Arancino Library uses a simple protocol, called **Cortex**, to communicate with the Arancino Module over serial connection. Cortex Protocol is designed to be easy to read and processed. Arancino Library, Arancino Module and Cortex Protocol are designed to be monodirectional and synchronous. In this scenario the Arancino Library within the microcontroller acts as *master*, and the Arancino Module as *slave*.


Each command sent using Cortex Protocol is composed by a *command identifier* and one or more *parameters*. *Command identifiers* and *parameters* are separated by a separator character, and the command ends with an end character. Each command sent will receive a response, formed by a Response Code and one or more returned values. All the items are separated by the same separtor character and ends with the same end character. The commands receive Response within a timeout interval of `100ms`. If no Response is received, the Command will be skipped.

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
**Important**: Do not use these character codes to compose String values to pass to the API

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
As exaplained above, when an API function is called, a command is sent over the `SerialUSB` and a response is received.
In the next paragraphs, for simplicity we are considering each command returns an *OK* response and using the following representation for *Separator Codes*:
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

#### flush
- Command Sent: `FLUSH@`
- Response Received: `100@`



## Reserved Keys

**PAY ATTENTION: NOT EVERYTHING DESCRIBED BELLOW IN THIS PARAGRAPH IS YET AVAILABLE**



Arancino Library uses some reserved keys to store environmental data into Redis. Values stored in those keys can be used for multiple scopes in user space. By default the Arancino Library uses those reserved keys to communicate in a synchronous way but it can do also in an asynchronous way.

Following the list of reserved keys:

| Reserved Key/Channel    | Version | Mode   | Description    |
| ----------------------- | ------- | ------ | -------------- |
| `___POWER___`           | ???     | Both   | The value can be `battery` or `power`, based on the power supply type. Very useful e.g. when switching from power to battery and automatically softly shoutdown the system. It works only with the specific hardware module for the Arancino Board.|
| `___MONITOR___`         | >=v0.1.0| Both   | used by `print` and `println` API |
| `__VERSION___`          | >=v0.1.0| Synch  | Arancino Library version |


To access data stored at reserved keys you have to use the Redis `get` command for _Sync_ mode and Redis `Subscribe` for Asynch mode (in _Async_ mode the _key_ name represents the _channel_ name).

By default the Arancino Library uses the _Sync_ mode, but you can change it to work in _Async_ mode or _Both_ (_Sync_ and _Async_ at the same time). To do that you have to define a variable called `RSVD_COMM` at the beginning of your program, with three possible values:

- `0` →  _Sync_
- `1` →  _Async_
- `2` →  _Both_

Example:
```c++
#include <Arancino.h>

#define RSVD_COMM 1

void setup() {
  //set up something
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


## Credits
Most of the documentation has been extrapolated from [Redis Command](https://redis.io/commands/)
Redis and Arduino are trademarks of their respective owners.
