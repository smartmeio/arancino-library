# `DEL`
Delete the specified *keys*. 

>Note: 
>
>- A *key* is ignored if it does not exist.
>- The specified *keys* will be deleted even if they were previously stored in a *persistent* way based on the `pers` configuration. You can only delete "application" keys (`"type":"appl"`)

## **Command**

**Arguments:**

| Args Type   | Desc      | Mandatory |
| ----- | --------- | --------- |
| `items`: String Array | Array of *keys* to delete | **Yes**       |

**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `pers:` Number    | Allows to del keys from the persistent datatstore. To use only with `"type": "appl"`. Allowed values: `0` is *False* and `1` is *True*  | **No**  |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr`: String | Signature used for ECDSA Authentication | **No**: used only in *Secure Mode* |

**Example:**

```json
    {
        "cmd": "DEL",
        "args":{
            "items":[
                "<key1>"
            ]
        },
        "cfg":{
            "ack": 1,
            "pers": 0,
            "sgntr": "<Signature>"
        }
    }
```

```json
    {
        "cmd": "DEL",
        "args":{
            "items":[
                "<key-1>", "<key-2>", "<key-n>"
            ]
        },
        "cfg":{
            "ack": 0,
            "pers": 1,
            "sgntr": "<Signature>"
        }
    }
```

## **Response**

**Arguments:** Empty
| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `keys`: Number | Number of deleted keys | **Yes**       |

**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng` | Challenge useful to sign the next command | No: used only in Secure Protocol |

**Examples:**

```json
    {
        "rsp_code": "100",
        "args":{
            "keys": <n>
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `DEL`: `200`,`206`	**Da controllare per aggiunta anche di errori presenti durante il protocollo sicuro
