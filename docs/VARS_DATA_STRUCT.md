## Data structures
### ArancinoConfig
`ArancinoConfig` is a class composed only by attributes. It is used to configure the Arancino Library behaviour. It replaces the use of direct configuration arguments of the `begin` function. Following the Arancino Config Attributes with their default values:
```c++
 int DECIMAL_DIGITS = 4;
 int SERIAL_TIMEOUT = 100;
 bool USE_PORT_ID_PREFIX_KEY = false;
 bool FREERTOS_LOOP_TASK_ENABLE = false;
 int FREERTOS_LOOP_TASK_STACK_SIZE = 256;
 int FREERTOS_LOOP_TASK_PRIORITY = 0;
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
### ArancinoMetadata
`ArancinoMetadata` is a data structure used to encapsulate firmware metadata; it is later read and exposed by Arancino Daemon via Rest API.
#### Content
```c++
typedef struct {
 char* fwname;
 char* fwversion;
 char* tzoffset;
} ArancinoMetadata;
```
#### Variables
* `fwname`: represents the firmware name;
* `fwversion`: represents the firmware version;
* `tzoffset`: is the local timezone offset. It is used to calculate the timestamp at which the firmware is compiled;
### ArancinoPacket
`ArancinoPacket` is a data structure returned from some Arancino Library APIs and contains details about the result.
##### Content
```c++
typedef struct {
 bool isError;
 int responseCode;
 int responseType;
 ArancinoResponse response;
} ArancinoPacket;
```
##### Variables
* `isError`: indicates the outcome of an API call. Its value is `False` (0) when everything is OK or `True` (1) when an error occurs. Both in positive and negative cases is possible to check `responseCode` for more details;
* `responseCode`: contains a code relative to API call outcome, identified by a label as follows (codes from `100` are defined by the [Cortex Protocol](docs/CORTEX.md#response-codes):
 |Name                           | Code  | Type      | Description | Note |
 |-------------------------------|-------|-----------|-------------|------|
 | RESERVED_KEY_ERROR            | `-3`    | Error     | Returned when trying to set a reserved key ||
 | COMMUNICATION_ERROR           | `-2`    | Error     | Returned when an UART communication error occur||
 | GENERIC_ERROR                 | `-1`    | Error     | Returned on generic and not specific error||
 | RSP_OK                        | `100`   | Success   | Generic operation successfully completed ||
 | RSP_HSET_NEW                  | `101`   | Success   | Setted value into a new field ||
 | RSP_HSET_UPD                  | `103`   | Success   | Setted value into an existing field ||
 | ERR                           | `200`   | Error     | Generic Error | |
 | ERR_NULL                      | `201`   | Error     | Retrivied Null Value | Deprecated|
 | ERR_SET                       | `202`   | Error     | Error during SET command ||
 | ERR_CMD_NOT_FND               | `203`   | Error     | Command not found ||
 | ERR_CMD_NOT_RCV               | `204`   | Error     | Error during SET command ||
 | ERR_CMD_PRM_NUM               | `205`   | Error     | Invalid parameter number ||
 | ERR_REDIS                     | `206`   | Error     | Generic Redis Error ||
 | ERR_REDIS_KEY_EXISTS_IN_STD   | `207`   | Error     | Key exists in the Standard Data Store ||
 | ERR_REDIS_KEY_EXISTS_IN_PERS  | `208`   | Error     | Key exists in the Persistent Data Store ||
 | ERR_NON_COMPATIBILITY         | `209`   | Error     | Non compatibility between Arancino Daemon and Library ||
 | ERR_INVALID_ARGUMENTS         | `210`   | Error     | Generic Invalid Arguments ||
 | ERR_VALUE                     | `211`   | Error     | Invalid Value ||
 | ERR_NOT_IMPLEMENTED           | `212`   | Error     | Not yet implemented functionality ||
* `responseType`: contains a code that indicate the type of the reponse, identified as follows:
  
 |Type                           | Code  | Description | Note |
 |-------------------------------|-------|-------------|------|
 | `VOID`                        | `0`   | returned when the API call does not provide any data, e.g. `set` command (see `responseCode` to evaluate the outcome)| |
 | `INT`                         | `1`   | returned when the Redis response is an integer value, e.g. `del` command | |
 | `STRING`                      | `2`   | returned when the Redis response is a string, e.g. `get` command | |
 | `STRING_ARRAY`                | `3`   | returned when the Redis response is an array string, e.g. `keys` command | |
* `response`: is a data structure that contain the Redis response. Its content can be an `int`, `char*` or `char**`, for example:
 * `response.integer`: interprets the content as an `int` type;
 * `response.string`: interprets the content as an `char*` type;
 * `response.stringArray`: interprets the content as an `char**` type;
> **Note**: when the `ArancinoPacket` returned from an API contains a string or an array string, the user must manually free the ArancinoPacket (or directly the `char*` or `char**` associated pointer) using the [`Arancino.free()`](docs/API.md#free) API; this is necessary to avoid memory leaks.

### ArancinoIface
`ArancinoIface` is an abstract class from which all communication interfaces inherit from. Depending on the used derivate class, different communication mediums may be used for the Arancino Protocol.
#### SerialIface
The `SerialIface` class provides the default UART communication medium between the MCU and the Arancino module.
##### API
```c++
void setSerialPort(Stream&  serialPort);
void setSerialPort();
```
The `setSerialPort` method sets a Stream (or the default Serial port if no argument is provided) as output stream for the Arancino protocol.

#### MQTTIface
The `MQTTIface` class uses the MQTT protocol in order to send and receive messages from the Arancino module. Special topics will be used in order to implement a bidirectional communication over the protocol.
##### API
```c++
void setNetworkClient(Client&  networkClient);
void setUsername(char*  username);
void setPassword(char*  password);
void setDaemonID(char*  daemonID);
void setBrokerAddress(char*  broker);
void setPort(int  port);
```
Most of the API is self explainatory, however particuar attention should be paid to both the `setNetworkClient` method, in which a valid network client should be passed as argument, and the `setBrokerAddress`, in which both IP addresses and domain names may be passed as string.
The `DaemonID` is the ID used by the Arancino module that will have to listen to the MCU requests; you can modify that in the configuration file of the Arancino module configuration file itself.

#### BluetoothIface
The `BluetoothIface` class exploits UART-like communication over Bluetooth and BLE.
##### API
```c++
void  setBLESerial(Stream&  bleUart);
```
A valid BLE stream should be passed as argument for the interface to properly work. Keep in mind that, depending on your implementation, you may need to check whether the exposed `Service` and `Characteristic` UUID match the ones contained in the Arancino module itself in the configutation file.