# Arancino Cortex Protocol

Arancino Cortex is a simple protocol enabling the communication between two (or more) parts. Its name remember the cerebral cortex of the brain, which has the purpose to put in communication the right and the left hemispheres of the brain.

Cortex could be used over serial, tcp, and other kind of connections, and it's used to make Arancino.cc Devices able to receive data from other devices (like Arancino Mignon) even simultaneously. In this case, Arancino.cc Board is intended as the right hemisphere, while the devices as if they were the "senses" of the left hemisphere.

Due to its design, Cortex is monodirectional and synchronous; in this scenario the Arancino.cc Mignon (or other compatible devices) within a microcontroller acts as *Peripheral Unit*, and the Arancino.cc Board as *Main Unit*. The *Peripheral* device sends a *Command* and waits the *Main* for a *Response*, so for each *Command* there's a *Response*.

Generically, could be more than one *peripheral unit* and only one *Main*, or at the minimum one *Main* and one *Peripheral*. As mentioned above the *Peripheral* sends one *Command* at time and receives a *Response* for each *Command*. Cortex *Commands* basically follows [Redis commands](https://redis.io/commands). *Commands* are mainly for storing data, and the storing can be setted to be *persistent* or *volatile*, for each *Command*. *Persistent* means that data can be retrievied also after a power off and on of *Main* device; *volatile* means that data are erased after a power off and on.

## **Secure Authentication and Communication**

Secure authentication between *peripheral units* and a *Main* allows to start a connection with the *Main* only to *peripheral units* previously choice , to achieve this is used the ECDSA (Elliptic Curve Digital Signature Algorithm). The *Main* receive device and signer certificates with the `START` command, verify the certificates validity and the presence of device public key in a whitelist, finally send back the response of the `START` command with a *challenge* (a random number codified with base64 encryption) in the configuration. When the *peripheral unit* receive the response with the *challenge* sign it with its private key and send it back in the `SIGN` command configuration (the *signature*), codified with base64 encryption. The *Main* receive the `SIGN` command and verify the correctness of the *signature*, then if it's verified the *peripheral unit* is authenticated and a *challenge* is send back in the response configuration.

Secure communication is implemented with the help of the *challenge* present in the previous response configuration, every time that a command is sent from the *peripheral unit* it has to contain the *signature* of the *challenge* received with the previous *response*, starting with the *challenge* received in the `SIGN` *response*.

The secure authentication and communication is used only when the flag `SCR_MOD` in the `START` response is 1

## Cortex Commands and Responses

Cortex *Commands* are composed by *Command Identifier*, *Command Arguments* and *Command Configuration* meanwhile a *Response* is composed by *Response Code*,  *Response Arguments* and *Response Configuration*.

The format used to build both *Commands* and *Responses* is Json. Before the transmission, *Commands* and *Responses* are converted to [**MessagePack**](https://msgpack.org), an efficient binary serialization format smaller and faster than Json. 

- *Command Identifier* is used to identify the right action to execute in the *Main* device.

- *Command Arguments* contains the parameters useful to execute the action identified by the *Command Identifier*.

- *Command Configuration* has extra parameters for the configuration of *command* or *response*.

- *Response Code* is a numeric code that indicates the success or the failure of a *Command*.

- *Response Arguments* when used, contains the values of required by a *Command*.

- *Response Configuration* used to send settings to the "peripheral" device for a specific "Command".


### Notes on Commands Configuration:

#### Persistent
Persistent configuration, with the `pers` key, is used to indicates that the value will be stored persistently. The default value is `0`.
#### Type
Type configuration, with the `type` key, stands to indicates the kind of the key. At this version of protocol `type` has three possible values:
- Applicaiton Type (`appl`): it's the most used case, used when users want to set a value within application. `pers` configuration can only be used with this `type`. `appl` is the default value for `type` configuration.
- Setting Type (`stng`): used to store configuration parameters for the application i.e. mqtt host and port. This kind of key should be stored *persistenly*. if `type:stng`, `pers` configuration will be ignored.
- Reserved Type (`rsvd`): used under the hood to set key for operation purpose. if `type:rsvd`, `pers` configuration will be ignored.
#### Ack
Ack configuration must be understood as the will to want to receive a *Response* to the *Command*. There are cases in which to speed up communication it may be useful not to send a reply, to do so just set this parameter to *False*.

#### Signature
Signature configuration is used when the communication is setted to be *Secure*. It contains the signature and by default expects a *Response* with the challenge, so the `ack` configuration will be ignored.


### Commands and Responses

Commands List:
- [`START`](START.md)
- [`SIGN`](SIGN.md)
- [`SET`](SET.md)
- [`GET`](GET.md)
- [`DEL`](DEL.md)
- [`HSET`](HSET.md)
- [`HGET`](HGET.md)
- [`HDEL`](HDEL.md)
- [`STORE`](STORE.md)
- [`STORETAGS`](STORETAGS.md)
- [`PUB`](PUB.md)
- [`FLUSH`](FLUSH.md)

### Response Codes

| `rsp_code` | |Description                                       |
|:-----------------:|:-------|:------------------------------------------|
| `100`             | **OK** | Generic operation successfully completed. |
| `101`             | **OK** | Set value into a new field.           |
| `102`             | **OK** | Set value into an existing field      |
| `200`             | **KO** | Generic Error                             |
| `201`             | **KO** | Retrieved NULL value (deprecated)         |
| `202`             | **KO** | Error during *SET* command                |
| `203`             | **KO** | Command not found                         |
| `204`             | **KO** | Command not received                      |
| `205`             | **KO** | Invalid parameter number                  |
| `206`             | **KO** | Generic Redis Error                       |
| `207`             | **KO** | Key exists in the Standard Data Store      |
| `208`             | **KO** | Key exists in the Persistent Data Store   |
| `209`             | **KO** | Non compatibility between Arancino Module and Library |
| `210`             | **KO** | Invalid arguments: incorrect, empty, or not equal number  |
