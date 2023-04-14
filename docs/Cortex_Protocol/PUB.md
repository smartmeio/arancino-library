#### `PUB`
Posts a message to the given channel. Return the number of clients that received the message.

##### **`PUB` Command**

**Arguments:** 

| Key       | Desc                                 | Mandatory |
| --------- | ------------------------------------ | --------- |
| `channel` | Channel where to publish the message | Yes       |
| `message` | Message to publish                   | Yes       |

**Configuration:**

| Key     | Desc                                    | Mandatory                        |
| :------ | :-------------------------------------- | -------------------------------- |
| `ack:` Number | Indicates whether a *response* (the ack) should be sent for the *command*. *True* by default. Allowed values: `0` is *False* and `1` is *True*| **No** | 
| `sgntr` | Signature used for ECDSA Authentication | No: used only in Secure Protocol |

**Example:**

```json
    {
        "cmd": "PUB",
        "args":{
            "items":[
                {"channel": "<channel-1>", "message": "<message-A>"},
                {"channel": "<channel-2>", "message": "<message-B>"}
            ]
        },
        "cfg":{
            "ack": 1,
            "sgntr": "<Signature>"
        }
    }
```

##### `PUB` Response

**Arguments:** 

| Key         | Desc                                         | Mandatory |
| ----------- | -------------------------------------------- | --------- |
| `num_client` | Number of clients that received the message. | Yes       |

**Configuration:**

| Key     | Desc                                      | Mandatory                        |
| ------- | ----------------------------------------- | -------------------------------- |
| `chlng` | Challenge useful to sign the next command | No: used only in Secure Protocol |

**Examples:**

```json
    {
        "rsp_code": "100",
        "args":{
            "clients": <Number_of_clients_that_received_the_message>
        },
        "cfg":{
            "chlng": "<challenge>"
        }
    }
```

>Note:
>
>- List of possible Error Codes for `PUB`: `200`, `206`	**Da controllare per l'aggiunta anche di errori presenti durante il protocollo sicuro

---