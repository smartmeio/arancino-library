# `SIGN`

When *Secure mode* is enabled, the `SIGN` *Command* represent the second and last part of the `START` *Command*. It means to complete the ECDSA Authentication. It sends back to the *Main* the challenge received as *Response* for the `START` *Command*, signed with the *Peripheal* device's private key. 

>Note: 
>
>- Present only after the `START` command and only in _Secure Mode_.

## Command

**Arguments:** None

**Configurations:**

| Key   | Desc                                    | Mandatory |
| ----- | --------------------------------------- | --------- |
| sgntr | Signature used for ECDSA Authentication | **Yes**     |

**Example:**

```json
    {
        "cmd": "SIGN",
        "args":{},
        "cfg":{
        	"sgntr": "<Signature>",
        }
    }
```

## Response

**Arguments:** None

**Configurations:**

| Key     | Desc                                      | Mandatory |
| ------- | ----------------------------------------- | --------- |
| `chlng` | Challenge useful to sign the next command | **Yes**     |

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
>- List of possible Error Codes for `SIGN`: `???`, `???`
