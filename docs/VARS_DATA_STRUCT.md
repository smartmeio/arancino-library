## Data structures

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

### ArancinoConfig
`ArancinoConfig` is a class composed only by attributes. It is used to configure the Arancino Library behaviour. It replaces the use of direct configuration arguments of the `begin` function.
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
