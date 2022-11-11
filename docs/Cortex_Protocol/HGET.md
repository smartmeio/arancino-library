# `HGET`
Returns the values associated with fields in the hashes stored at keys.

## **Command**

**Arguments:** 

| Key               | Desc                       | Mandatory |
| ----------------- | -------------------------- | --------- |
| `items:` Array        | Array of `key-field` pairs    | **Yes**       |
| - `key:` String       | Name of the *key*             | **Yes**       |
| - `field:` String     | Name of the *field*           | **Yes**       |

**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `pers:` Number    | Allows to get values from the persistent datastore. To use only with `"type": "appl"`. Allowed values: `0` is *False* and `1` is *True*  | **No**  |
| `type:` String    | Represent the kind of the key. Availalble values are `appl:` for application key (default value), `stng` for application settings key, `rsvd` for reserved key used under the hood for operation.  | **No** |                 |
| `sgntr:` String | Signature used for ECDSA Authentication | **No**: used only in *Secure Mode* |



**Example:**

Retrieving single *field*

```json
    {
        "cmd": "HGET",
        "args":{
            "items":[
                {"key": "<key1>", "field": "<field-A>"}
            ]
        },
        "cfg":{
            "pers": 0,
            "type": "appl",
            "sgntr": "<Signature>"
        }
    }
```

Retrieving multiple *fields*

```json
    {
        "cmd": "HGET",
        "args":{
            "items":[
                {"key": "<key-1>", "field": "<field-A>"},
                {"key": "<key-1>", "field": "<field-B>"},
                {"key": "<key-2>", "field": "<field-C>"}
            ]
        },
        "cfg":{
            "pers": 0,
            "type": "appl",
            "sgntr": "<Signature>"
        }
    }
```

## **Response**

**Arguments:** 

| Key                     | Desc                                                         | Mandatory |
| ----------------------- | ------------------------------------------------------------ | --------- 
| `items:` Array | Array of `key-field-value` tuples retrieved from the *keys* and the *fields* specified as *command args* | **Yes**       |
| - `key:` String       | Name of the *key*             | **Yes**       |
| - `field:` String     | Name of the *field*           | **Yes**       |
| - `value:` String     | Value at the *key-field*      | **Yes**       |

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
                {"key": "<key-1>", "field": "<field-A>", "value": "<value-1>"},
                {"key": "<key-1>", "field": "<field-B>", "value": "<value-2>"},
                {"key": "<key-2>", "field": "<field-C>", "value": "<value-3>"}
            ]
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `HGET`: `200`, `206`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro
