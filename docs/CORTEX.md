## Cortex Protocol
Arancino Library uses a simple protocol, called **Cortex**, to communicate with the Arancino Module over serial connection. Cortex Protocol is designed to be easy to read and processed. Arancino Library, Arancino Module and Cortex Protocol are designed to be monodirectional and synchronous. In this scenario the Arancino Library within the microcontroller acts as *master*, and the Arancino Module as *slave*.


Each command sent using Cortex Protocol is composed by a *command identifier* and one or more *parameters*. *Command identifiers* and *parameters* are separated by a separator character, and the command ends with an end character. Each command sent will receive a response, formed by a Response Code and one or more returned values. All the items are separated by the same separtor character and ends with the same end character. The commands receive Response within a timeout interval of `100ms`. If no Response is received, the Command will be skipped.

### Commands identifier:


| API               | Command Identifiers    |
| ------------------ |:-------------:|
| [`begin`](#begin)  | START         |
| [`set`](#set)      | SET           |
| [`get`](#get)      | GET           |
| [`mset`](#mset)    | MSET          |
| [`mget`](#mget)    | MGET          |
| [`del`](#del)      | DEL           |
| [`keys`](#keys)    | KEYS          |
| [`hget`](#hget)    | HGET          |
| [`hset`](#hset)    | HSET          |
| [`hkeys`](#hkeys)  | HKEYS         |
| [`hvals`](#hvals)  | HVALS         |
| [`hdel`](#hdel)    | HDEL          |
| [`flush`](#flush)  | FLUSH         |
| [`publish`](#publish)    | PUB     |
| [`mset`](#mset)    | MSET     |
| [`mget`](#mget)    | MGET     |
| [`store`](#store)  | STORE    |
| [`storetags`](#storetags)| STORETAGS|

### Commands separator chars
**Important**: Do not use these character codes to compose string values to pass to the API

| Separator             | Char Code     |
| --------------------- |:-------------:|
| Command Sepatator     | `4`           |
| Array separator       | `16`          |
| NULL character        | `25`          |
| End of transmission   | `30`          |


### Response Codes
| Response Code  | Type      | Description | Note |
|-------|-----------|-------------|------|
| `100`   | Success   | Generic operation successfully completed ||
| `101`   | Success   | Setted value into a new field ||
| `103`   | Success   | Setted value into an existing field ||
| `200`   | Error     | Generic Error | |
| `201`   | Error     | Retrivied Null Value | Deprecated|
| `202`   | Error     | Error during SET command ||
| `203`   | Error     | Command not found ||
| `204`   | Error     | Error during SET command ||
| `205`   | Error     | Invalid parameter number ||
| `206`   | Error     | Generic Redis Error ||
| `207`   | Error     | Key exists in the Standard Data Store ||
| `208`   | Error     | Key exists in the Persistent Data Store ||
| `209`   | Error     | Non compatibility between Arancino Daemon and Library ||
| `210`   | Error     | Generic Invalid Arguments ||
| `211`   | Error     | Invalid Value ||
| `212`   | Error     | Not yet implemented functionality ||


### Commands and Protocol
As exaplained above, when an API function is called, a command is sent over the `SerialUSB` and a response is received.
In the next paragraphs, for simplicity we are considering each command returns an *OK* response and using the following representation for *Separator Codes*:

- Command Sepatator → `4`  → `#`
- Array separator → `16` → `%`
- End of transmission → `30`  →` @`

#### begin
- Command Sent: `START#<lib vers>#<fwname>#<fwversion>#<compiletime>#<coreversion>@`
- Response Received: `100@`

#### set
- Command Sent: `SET#<key>#<value>@`
- Response Received: `100@`

#### get
- Command Sent: `GET#<key>@`
- Response Received: `100#<value>@`

#### mset
- Command Sent: `MSET#<key-1>%<key-2>...%<key-n>#<val-1>%<val-2>...%<val-n>@`
- Response Received: `100#@`

#### mget
- Command Sent: `MGET#<key-1>%<key-2>...%<key-n>@`
- Response Received: `100#<val-1>#<val-2>...#<val-n>]@`

#### del
- Command Sent: `DEL#<key>@`
- Response Received: `100#1@`

#### keys
- Command Sent: `KEYS#<pattern>@`
- Response Received: `100[#<key-1>#<key-2>#<key-n>]@`

#### hset
- Command Sent: `HSET#<key>#<field>#<value>@`
- Response Received: 101@

#### hget
- Command Sent: `HGET#<key>#<field>@`
- Response Received: `100#<value>@`

#### hgetall
- Command Sent: `HGETALL#<key>@`
- Response Received: `100#<field-1>#<value-1>#<field-2>#<value-2>@`

#### hkeys
- Command Sent: `HKEYS#<key>@`
- Response Received: `100HKEYS#<key>@`

#### hvals
- Command Sent: `HVALS#<key>@`
- Response Received: `100#<value-1>#<value-2>@`

#### hdel
- Command Sent: `HDEL#<key>#<field>@`
- Response Received: `100#1@`

#### flush
- Command Sent: `FLUSH@`
- Response Received: `100@`

#### storetag
- Command Sent: `STORE#<key>#<tag1>%<tag2>%<tag3>#<value1>%<value2>%<value3>#<timestamp>@`
- Response Received: `100@`

- Command Sent: `STORE#<key>#<tag1>%<tag2>%<tag3>#<value1>%<value2>%<value3>@`
- Response Received: `100@`

#### store
- Command Sent: `STORE#<key>#<values>@`
- Response Received: `100#<timestamp>@`

- Command Sent: `STORE#<key>#<values>#<timestamp>@`
- Response Received: `100#<timestamp>@`
