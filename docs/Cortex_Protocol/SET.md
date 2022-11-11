# `SET`
Sets _keys_ to hold the string _values_. If _key_ already holds a _value_, it is overwritten, regardless of its type. `SET` works both in _persistent_ and in _volatile_ (_non-persistent_) but not at the same time.

## **Command**

**Arguments:**

| Args Type     | Desc                           | Mandatory |
| ------- | ------------------------------ | --------- |
| `items:` Array         | Array of `key-value` pairs    | **Yes**       |
| - `key:` String       | Name of the *key*             | **Yes**       |
| - `value:` String     | Value of the *key*            | **Yes**       |



**Configurations:**

| Key     | Desc                                                         | Default | Mandatory |
| :------ | :----------------------------------------------------------- |: -------|:--------- |
| `type:` String    | Represent the kind of the key. Availalble values are `appl` for application key, `stng` for application settings key, `rsvd` for reserved key used under the hood for operation.  |  `appl` |**No** |
| `pers:` Number    | Allows to store values in a persistent way. To use only with `"type": "appl"`. Allowed values: `0` is *False* and `1` is *True*  | `0` (*False*) |**No**  |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| | `1` (*True*) |**No** | 
| `sgntr:` String | Signature used for ECDSA Authentication                     | **No**: used only in *Secure Mode* |

**Examples:**

Example of `SET` command with a single _key-value_ pair stored non-persistently
```json
    {
        "cmd": "SET",
        "args": {
            "items":[
                {"key": "<key>", "value": "<value>"}
            ]
        },
        "cfg":{
            "type": "appl",
            "pers": 0,
            "ack": 1,
            "sgntr": "<signature>"
        }
    }
```

Example of `SET` with a single _key-value_ pair stored persistently
```json
    {
        "cmd": "SET",
        "args": {
            "items":[
                {"key": "<key>", "value": "<value>"}
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

Example of `SET` with a multiple _key-value_ pairs that are used as configuration, or application settings, values:
```json
    {
        "cmd": "SET",
        "args":{
            "items":[
                {"key": "<setting-key-1>", "value": "<value-1>"},
                {"key": "<setting-key-2>", "value": "<value-2>"},
                {"key": "<setting-key-n>", "value": "<value-n>"}
            ]
        },
        "cfg":{
            "type": "stng",
            "sgntr": "<Signature>"
        }
    }
```

## **Response**

**Arguments:** None

**Configuration:**

| Key     | Desc                                      | Mandatory               |
| ------- | ----------------------------------------- | ----------------------- |
| `chlng:` String | Challenge useful to sign the next command | **No**: used only in *Secure Mode* |

**Examples:**

Example of a response of a `SET` executed successfully.
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
>- List of possible [Error Codes](#response-codes) for `SET`: `200`, `201`, `206`, `207`, `208`, `210`
