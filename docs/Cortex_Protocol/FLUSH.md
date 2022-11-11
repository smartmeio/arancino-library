#### `FLUSH`
Delete all the keys. 

>Note: 
>
>- This command never fails.

## **Command**

**Arguments:** Empty

**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `pers:` Number  | Allows to delete persistent or volatile keys. False by default. Allowed values: `0` is _False_ and `1` is _True_  | **No**                     |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr`: String | Signature used for ECDSA Authentication | No: used only in Secure Protocol |

**Example:**

```json
    {
        "cmd": "FLUSH",
        "args":{},
        "cfg":{
            "pers": 0,
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
| `chlng`: String | Challenge useful to sign the next command | No: used only in Secure Protocol |

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
>- List of possible Error Codes for `FLUSH`: `200`, `206`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro
