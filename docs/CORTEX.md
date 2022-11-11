## Cortex Protocol
Arancino Library uses a simple protocol, called **Cortex**, to communicate with the Arancino Module using multiple connection Interfaces (MQTT,Serial,Bluetooth). Cortex Protocol is designed to be easy to read and processed. Arancino Library, Arancino Module and Cortex Protocol are designed to be monodirectional and synchronous. In this scenario the Arancino Library within the microcontroller acts as *master*, and the Arancino Module as *slave*.

Each command sent using Cortex Protocol is composed by a *command identifier* and one or more *parameters*. *Command identifiers* and *parameters* are formatted inside a *JSON* and the message itself is compressed into a *MessagePack*. Each command sent will receive a response, formed by a Response Code and one or more returned values. The commands receive Response within a timeout interval. If no Response is received, the Command will be skipped.

### Commands and Protocol
As exaplained above, when an API function is called, a command is sent over the `ArancinoInterface` and a response is received.
More details about the command structure and relative responses may be found in the [Cortex Protocol](Cortex_Protocol/README.md) section.