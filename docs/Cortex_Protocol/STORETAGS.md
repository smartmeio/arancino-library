#### `STORETAGS`
Store tags for a Time Series

## **Command**

**Arguments:** 

| Args Type| Desc                             | Mandatory |
| -------- | -------------------------------- | --------- |
| `key:` String             | *Key* to which apply the *tags*           | Yes       |
| `ts:` Number       | UNIX timestamp                            | No        |
| `items:` Array              | Array of *name-value* *tags*              | Yes       |
| - `name:` String           | Name of the *tag* to apply at the *pers*   | Yes       |
| - `value:` String           | Value of the *tag* to apply at the *pers* | Yes       |


**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr:` String | Signature used for ECDSA Authentication | No: used only in Secure Protocol |

**Example:**

```json
    {
        "cmd": "STORETAGS",
        "args":{
            "key": "<key-1>",
            "items": [ 
                {"tag": "<tag-1>", "value": "<value-1>"}, 
                {"tag": "<tag-2>", "value": "<value-2>"}, 
                {"tag": "...", "value": "..."}
            ],
            "ts": "<UNIX timestamp>"
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
        "args":{},
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `STORETAGS`: `200`, `206`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro

---
