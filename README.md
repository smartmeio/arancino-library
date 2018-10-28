# Arancino Library
Arancino Library works on Arancino boards, it's written in Arduino lang and can be imported in Arduino IDE. Arancino Library it's written to runs on SAMD21 Microcontroller over SAMD Arduino platform. Arancino Library uses the serial connection to communicate with Arancino Module which runs in the linux side of the Arancino board.

Arancino Library allows to export/import data to/from the Linux enviroment using it as key-value datastore. The API of Arancino Library API are modeled on redis commands.

TODO: which Serial of Arduino it uses and which Baudrate

## Getting Started
TODO: how to import in Arduino IDE
TODO: first example


## Arancino Library API

### *void begin(int timeout)* 
TODO

##### Parameters
* **`timeout`**: TODO:

##### Example
TODO:



___
### *void set( String key, String [ int | float] value )*
Set *key* to hold the string *value*. If *key* already holds a *value*, it is overwritten, regardless of its type.

##### Parameters
* **`key`**: the *key* name 
* **`value`**: the *value* for the specified *key*. can be String, int o float  

##### Example

```C++
Arancino.set("humidity",67.5);
```



___
### *String get( String key )*
Get the *value* of *key*. If the *key* does not exist TODO:~~the special value nil is returned.~~ 

#### Parameters
* **`key`**: the name of the key from which retrieve the value

#### Return value
XXX TODO

##### Example



___
### *int del( String key )*
Removes the specified *key*. A *key* is ignored if it does not exist.

##### Parameters
* **`key`**: the name of the key to delete. 

##### Return value
int reply: The number of keys that were removed.

##### Example
TODO



___
### *String\* keys(String pattern)*
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
XXX TODO



___
### *int hset( String key, String field , String [ int | float] value )*
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
XXX TODO



___
### *String hget( String key, String field ) {*
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
### *String\* hgetall( String key )*
Returns all fields and values of the hash stored at *key*. In the returned value, every *field* name is followed by its *value*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### *String\* hkeys( String key )*
Returns all field names in the hash stored at key.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### *String\* hgetvals( String key )*
Returns all values in the hash stored at *key*.

##### Parameters
* **`key`**: the name of the *key* which hold the hash.

##### Return value
*String reply:
TODO

##### Example
TODO



___
### *int hdel( String key, String field )*
Removes the specified *field* from the hash stored at *key*. ~If specified *field* that does not exist within this hash, this commands returns 0~. If key does not exist, it is treated as an empty hash and this command returns 0.
TODO check if correct



##### Parameters
* **`key`**: the name of the *key* which hold the hash.
* **`field`**: the name of the *field* to delete.

##### Return value
int reply: the number of fields that were removed from the hash.

##### Example
TODO



## Protocol
Arancino Library use simple protocol to communicate with Arancino Module over the serial connection. 

TODO: describe protocol

## Debug 
It's a feature introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1). Debug mode can be enabled in Arancino Board by setting the GPIO `XXXX`; after setting the Debug GPIO, power on Arancino Board (or resetting the microtroller if previously powered on) Debug data will be send over the `SERIAL???` and you can read it trough a serial console, like cu, screen or the Arduino IDE Serial Monitor.

### Debug Message
TODO

### Responde Code
Each command send via the API, will receive a response. Response contains the Response code and the Response value.




## Credits
Much of the documentation has been extrapolated from [Redis Command](https://redis.io/commands/)