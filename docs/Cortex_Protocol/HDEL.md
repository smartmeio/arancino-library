# `HDEL`
Removes the specified field from the hash stored at key.  

>Note: 
>
>- Specified fields that do not exist within this hash are ignored.

## **Command**

**Arguments:** 

| Key     | Desc                                     | Mandatory |
| ------- | ---------------------------------------- | --------- |
| `items:` Array | Array of `key-field` pairs to remove| **Yes**       |
| - `key:` String       | Name of the *key*             | **Yes**      |
| - `field:` String     | Name of the *field*           | **Yes**      |


**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `pers:` Number    | Allows to get values from the persistent datastore. To use only with `"type": "appl"`. Allowed values: `0` is *False* and `1` is *True*  | **No**  |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr:` String | Signature used for ECDSA Authentication | No: used only in Secure Protocol |

**Example:**

```json
    {
        "cmd": "HDEL",
        "args":{
            "items":[
                {"key": "<key-1>", "field": "<field-A>"},
                {"key": "<key-1>", "field": "<field-B>"}
                {"key": "<key-2>", "field": "<field-A>"}
                {"key": "<key-2>", "field": "<field-B>"}
            ],
        },
        "cfg":{
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

## Response

**Arguments:** 

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `fields`: Number | Number of deleted fields | **Yes**       |

**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng:` String | Challenge useful to sign the next command | No: used only in Secure Protocol |

**Examples:**

```json
    {
        "rsp_code": "100",
        "args":{
            "fields": <n>
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `HDEL`: `200`, `206`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro
