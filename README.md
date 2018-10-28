# Arancino Library
Arancino Library works on Arancino boards, it's written in Arduino lang and can be imported in Arduino IDE. Arancino Library it's written to runs on SAMD21 Microcontroller over SAMD Arduino platform. Arancino Library uses the serial connection to communicate with Arancino Module which runs in the linux side of the Arancino board.

Arancino Library allows to export/import data to/from the Linux enviroment using it as key-value datastore. The API of Arancino Library API are modeled on redis commands.

TODO: which Serial of Arduino it uses and which Baudrate

## Getting Started
TODO: how to import in Arduino IDE
TODO: first example


## Arancino Library API

### *void set( String key, String [ int | float] value )*
___
Set *key* to hold the string *value*. If *key* already holds a *value*, it is overwritten, regardless of its type.

#### Parameters
* **`key`**: the *key* name 
* **`value`**: the *value* for the specified *key*. can be String, int o float  

#### Example

```C++
Arancino.set("humidity",67.5);
```

### *String get( String key )*
___
Get the *value* of *key*. If the *key* does not exist TODO:~~the special value nil is returned.~~ 

#### Parameters
* **`key`**: the name of the key from which to retrieve the value

#### Return value
XXX TODO

##### Example
```C++
// Get the cpu usage. The cpu usage is written by a linux process into a key named cpu_usage 
String cpu = Arancino.get("cpu_usage");

//TODO:mostrare come gestire il ritorno
```

### *int del( String key )*
___
Removes the specified keys. A key is ignored if it does not exist. Return the num

#### Parameters
* **`key`**: the name of the key to delete. 

#### Return value
int reply: The number of keys that were removed.

#### Example
XXX TODO


### *String\* keys(String pattern)*
___
Returns all keys matching pattern.

Supported glob-style patterns:

- h?llo matches hello, hallo and hxllo
- h*llo matches hllo and heeeello
- h[ae]llo matches hello and hallo, but not hillo
- h[^e]llo matches hallo, hbllo, ... but not hello
- h[a-b]llo matches hallo and hbllo

Use \ to escape special characters if you want to match them verbatim.

#### Parameters
* **`pattern`**: the pattern used to find the matching keys.

#### Return value
String* reply: ~list~ of keys matching pattern.

#### Example
XXX TODO

### Hashtable Set
___

### Hashtable Get
___

### Hashtable Get All 
___

### Hashtable Get Keys
___

### Hastable Get Values
___

### Hashtable Delete
___


## Protocol
Arancino Library use simple protocol to communicate with Arancino Module over the serial connection. 
TODO: describe protocol

## Debug 
It's a feature introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1). Debug mode can be enabled in Arancino Board by setting the GPIO `XXXX`; after setting the Debug GPIO, power on Arancino Board (or resetting the microtroller if previously powered on) Debug data will be send over the `SERIAL???` and you can read it trough a serial console, like cu, screen or the Arduino IDE Serial Monitor.

### Debug Message

### Responde Code
Each command send via the API, will receive a response. Response contains the Response code and the Response value.


