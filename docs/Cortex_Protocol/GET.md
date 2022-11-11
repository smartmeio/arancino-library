# `GET`
Retrieves the *values* of *keys*. If the *key* does not exist the special value `null` is returned. 

>Note: 
>
>- An error is returned if the value stored at *key* is not a string, because `GET` only handles string values.

## Command**

##### **Arguments:**

| Args Type   | Desc      | Mandatory |
| ----- | --------- | --------- |
| `items`: String Array | Array of *keys* to retrieve | **Yes**       |

**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `pers:` Number    | Allows to get values from the persistent datastore. To use only with `"type": "appl"`. Allowed values: `0` is *False* and `1` is *True*  | **No**  |
| `type:` String    | Represent the kind of the key. Availalble values are `appl:` for application key (default value), `stng` for application settings key, `rsvd` for reserved key used under the hood for operation.  | **No** |                   |
| `sgntr`: String | Signature used for ECDSA Authentication | **No**: used only in *Secure Mode* |

**Example:**

Retrieving single *key* stored persistently
```json
    {
        "cmd": "GET",
        "args":{
            "items": [
                "<key-1>"
            ]
        },
        "cfg":{
            "pers": 1,
            "type": "appl",
            "sgntr": "<Signature>"
        }
    }
```

Retrieving multiple *key*
```json
    {
        "cmd": "GET",
        "args":{
            "items":[
                "<key-1>", "<key-2>", "<key-n>"
            ]
        },
        "cfg":{
            "pers": 0,
            "type": "appl",
            "sgntr": "<Signature>"
        }
    }
```

Retrieving a configuration/setting *key*
```json
    {
        "cmd": "GET",
        "args":{
            "items":[
                "<setting-key-1>"
            ]
        },
        "cfg":{
            "type": "stng",
            "sgntr": "<Signature>"
        }
    }
```

## Response

**Arguments:** 

| Args Type     | Desc                       | Mandatory |
| ------- | -------------------------- | --------- |
| `items:` Array  | Array of `Key-Value` pairs retrieved from the *keys* specified as *command* args | **Yes**       |
| - `key:` String    | Name of the *key*             | **Yes**       |
| - `value:` String  | Value of the *key*            | **Yes**       |


**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng:` String | Challenge useful to sign the next command | **No**: used only in *Secure Mode* |

**Examples:**

```json
    {
        "rsp_code": "100",
        "args":{
            "items":[
                {"key": "<key-1>", "value": "<value-1>"},
                {"key": "<key-2>", "value": "<value-2>"},
                {"key": "<key-n>", "value": "<value-n>"}
            ]
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `GET`: `200`, `201`, `206`, `207`, `208`, `210`	**Da controllare per aggiunta anche di errori presenti durante il protocollo sicuro
