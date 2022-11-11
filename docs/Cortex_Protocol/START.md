
# `START`
*Peripheral* unit sends a request to start the communication with the *Main* unit.

## Command

**Arguments:**
| Key        | Desc                        | Mandatory               |
|:--------------------|:----------------------------|-----------------------------|
|`port_id`: String            |The Unique ID of the *Port*/MCU| **Yes**       |
|`fw_mcu_family`: String      |The Family of the Microcontroller| **Yes**       |
|`fw_lib_ver`: String         |Version of the Library that implements Cortex Protocol on the MCU |**Yes** |
|`fw_name`: String            |Name of the firmware running on the microcontroller|**Yes**|
|`fw_ver`: String             |Version of the firmware running on the microcontroller|**Yes**|
|`fw_build_time` : String     |Build date and time of the firmware running on the microcontroller|**Yes**|
|`fw_core_ver`: String        |Version of the Arancino Core (Arduino) which the firmware is built on|**Yes**|
|`fw_crtx_ver`: String        |Version of this protocol|**Yes**|

**Configurations:**

| Key        | Desc                        | Mandatory               |
|:--------------------|:----------------------------|-----------------------------|
|`scr_mod`: Number  |Flag for _Secure Mode_, `1` is enabled `0` is disabled|**Yes**|
|`crt_sig`: String            |Signer Certificate |**No**: Used only in *Secure Mode*|
|`crt_dev`: String            |Device Certificate |**No**: Used only in *Secure Mode*|


>Notes: 
>- You could pass custom keys other the one above specified (see how `CUSTOM_KEY_1` and `CUSTOM_KEY_2` are passed in the _Command Arguments_ in the example below).
>- `fw_mcu_family` at the moment could have the following values depending on the architecture of the *slave* device: `SAMD21`, `STM32`, `NRF52`.
>- `fw_lib_ver`, `fw_ver`, `fw_core_ver`, `crtx_ver` must implement Sem Ver 2.0
>- `port_id` is retrivied with the [Microcontroller ID Arudino Library]([htt](https://github.com/smartmeio/microcontroller-id-library))

**Example:**

```json
    {
        "cmd": "START",
        "args":{
            "port_id": "<port unique identified>",
            "fw_mcu_family": "<mcu family>",
            "fw_lib_ver": "<firmware library version>",
            "fw_name": "<firmware name>",
            "fw_ver": "<firmware version>",
            "fw_build_time": "<firmware build time>",
            "fw_core_ver": "<firmware core version>",
            "fw_crtx_ver": "<firmware cortex version>",
            "CUSTOM_KEY_1": "CUSTOM_VALUE_1",
            "CUSTOM_KEY_2": "CUSTOM_VALUE_2"
        },
        "cfg":{
            "scr_mod": "<secure mode>",
            "crt_sig": "<Signer Certificate>",
            "crt_dev": "<Device Certificate>"
        }
    }
```


## Response
**Arguments:** None

**Configurations:**

| Key        | Desc                        | Mandatory               |
|:--------------------|:----------------------------|-----------------------------|
|`ts`: Number       |Timestamp used for synchronize the RTC of the Microcontroller|**Yes**|
|`log_lvl`: String  |The Level of the Log to be used|**Yes**|
|`dmn_ver`: String  |Version of the Daemon|**Yes**|
|`dmn_env`: String  |Running environment|**Yes**|
|`chlng`:String     |Challenge useful to sign the `SIGN` *Command* |**No**: Used only in *Secure Mode*|

>Note: 
> - Accepted values for `LOG_LVL` are: `ERORR`, `WARN`, `DEBUG` and `INFO`.
> - Accepted values for `DMN_ENV` are: `PROD`, `TEST` and `DEV`.
> - Version number must implement Sem Ver 2.0

**Examples:**

```json
    {
        "rsp_code": "100",
        "args": {
            "dmn_ver": "<daemon version>",
            "dmn_env": "<daemon environment>"
        },
        "cfg":{
            "ts": "<timestamp>",
            "log_lvl": "<log level>",
            "chlng": "<challenge>"
        }
    }
```

>Note:
> - List of possible [Error Codes](#response-codes) for `START`: `200`, `203`, `204`, `205`, `209`, `210`
