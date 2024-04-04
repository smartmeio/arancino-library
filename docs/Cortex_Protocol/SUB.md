# `SUB`
Subscribes to the specified *channels*.

## Command**

##### **Arguments:**

| Args Type   | Desc      | Mandatory |
| ----- | --------- | --------- |
| `items`: String Array | Array of *channels* to subscribe to | **Yes**       |

**Configuration:**

| Key     | Desc                                                        | Default | Mandatory |
| ------- |------------------------------------------------------------ |---------|---------- |
| `prfx`: Number | Allows to insert the *Peripheral* device id as prefix of the key/channel. Allowed values: `0` is *False* and `1` is *True*. | `0` (*False*) | **No** |
| `sgntr`: String | Signature used for ECDSA Authentication | | **No**: used only in *Secure Mode* |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 


**Example:**

Subscribe to two *channels*
```json
    {
        "cmd": "SUB",
        "args":{
            "items": [
                "<channel-1>", "<channel-2>"
            ]
        },
        "cfg":{
            "ack": 1,
            "prfx": 0,
            "sgntr": "<Signature>"
        }
    }
```

## Response

**Arguments:** None

**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng:` String | Challenge useful to sign the next command | **No**: used only in *Secure Mode* |

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