# `HSET`
Sets *field* in the hash stored at *key* to *value*.  

>Note: 
>
>- If *key* does not exist, a new *key* holding a *hash* is created.
>- If *field* already exists in the *hash*, it is overwritten.
> When setting multiple *key* and all (or some) of they were previously stored in a different way (persistent/volatile) than now, a warning will be received as response

## **Command**

**Arguments:** 

| Data | | Desc                             | Mandatory |
| ------------ |-| -------------------------------- | --------- |
| `items:` Array | Array of `key-field-value` tuple | **Yes**       |
| - `key:` String       | Name of the *key*             | **Yes**       |
| - `field:` String     | Name of the *field*           | **Yes**       |
| - `value:` String     | Value at the *key-field*      | **Yes**       |

**Configuration:**

|  Key | Desc                                                         | Mandatory                        |
| :----------- | :----------------------------------------------------------- | -------------------------------- |
| `type:` String    | Represent the kind of the key. Availalble values are `appl:` for application key (default value), `stng` for application settings key, `rsvd` for reserved key used under the hood for operation.  | **No** |
| `pers:` Number       | Allows to store values in a persistent way. False by default. Allowed values: `0` is _False_ and `1` is _True_ | **No**                              |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr:` String      | Signature used for ECDSA Authentication                      | **No**: used only in *Secure Mode* |

**Examples:**

Example of `HSET` command with a single _key-field-value_ set stored non-persistently


```json
    {
        "cmd": "HSET",
        "args":{
            "items":[   
                {"key": "<key-1>", "field": "<field-A>", "value": "<value-1>"}
            ]
        },
        "cfg":{
            "type": "appl",
            "pers": 0,
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```


Example of `HSET` command with a single _key-field-value_ set stored persistently

```json
    {
        "cmd": "HSET",
        "args":{
            "items":[   
                {"key": "<key-1>", "field": "<field-A>", "value": "<value-1>"}
            ]
        },
        "cfg":{
            "type": "appl",
            "pers": 1,
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

Example of `HSET` command with a single _key-field-value_ set of settings values:

```json
    {
        "cmd": "HSET",
        "args":{
            "items":[   
                {"key": "<setting-key-1>", "field": "<setting-field-A>", "value": "<value-1>"}
            ]
        },
        "cfg":{
            "type": "stng",
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

Example of `HSET` command with a multiple _key-field-value_ sets stored persistently.

```json
    {
        "cmd": "HSET",
        "args":{
            "items":[
                {"key":"<key-1>", "field":"<field-A>", "value":"<value-1>"},
                {"key":"<key-1>", "field":"<field-B>", "value":"<value-2>"},
                {"key":"<key-2>", "field":"<field-A>", "value":"<value-3>"},
                {"key":"<key-2>", "field":"<field-B>", "value":"<value-4>"}
            ]
        },
        "cfg":{
            "type": "appl",
            "pers": 1,
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

## **Response**

**Arguments:** None


**Configuration:**

| Required Key | Desc                                      | Mandatory                        |
| ------------ | ----------------------------------------- | -------------------------------- |
| `chlng:` String      | Challenge useful to sign the next command | **No**: used only in *Secure Mode* |

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
>- List of possible Error Codes for `HSET`: `200`, `206`, `207`, `208`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro