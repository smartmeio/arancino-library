# `STORE`
Store one or multiple value in a Time Series data structure at key

## **Command**

**Arguments:** 

| Key     | Desc                           | Mandatory |
| ------- | ------------------------------ | --------- |
| `Key`   | Redis key                      | Yes       |
| `Value` | Redis value related to the key | Yes       |
| `Ts`    | UNIX timestamp                 | No        |


| Args Type     | Desc                           | Mandatory |
| ------- | ------------------------------ | --------- |
| `items` Array   | Array of `key-value-timestamp` sets    | **Yes**       |
|   | - `key:` String| **Yes** |
|   | - `value:` Number | **Yes** |
|   | - `timestamp:` Number | **No** |


**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr` | Signature used for ECDSA Authentication | No: used only in Secure Protocol |

**Example:**

Example of `STORE` command with a single _key-field-timestamp_ set

```json
    {
        "cmd": "STORE",
        "args":{
            "items":[
                {"key": "key-1", "value": "value-1", "ts": "timestamp-1"}
            ]
        },
        "cfg":{
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

Example of `STORE` command with a multiple *key-value-timestamp* sets

```json
	{
        "cmd": "STORE",
        "args":{
            "items":[
                {"key": "key-1", "value": "1", "ts": "timestamp-1"},
                {"key": "key-2", "value": "2", "ts": "timestamp-2"},
                {"key": "key-3", "value": "3.14", "ts": "timestamp-3"},
                {"key": "...", "value": "...", "ts": "..."}
            ]
        },
        "cfg":{
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

## **Response**

**Arguments:** None

**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng` | Challenge useful to sign the next command | No: used only in Secure Protocol |

**Examples:**

```json
    {
        "rsp_code": "100",
        "args":{
            "items": ["<timestamp-1>", "<timestamp-2>", "<timestamp-n>"]
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `STORE`: `200`, `206`, `211`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro
