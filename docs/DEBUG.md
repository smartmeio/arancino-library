## Debug
This feature was introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1) and can be enabled in the Arancino Board by setting the `GPIO PA31` - Pin number `26`; after setting the Debug GPIO, power on the Arancino Board (or resetting the microtroller if previously powered on), the Debug data will be sent over the 'Serial` and you can read it through a serial console, like cu, screen or the Arduino IDE Serial Monitor.

### Responde Code
Each command sent via the API, will receive a response. Response contains the _Response Code_ and one or more _Response Value_. See [Commands and Protocol](#Commands and Protocol)

### Debug Messages
Debug messages are similar to those written above in the [Commands and Protocol](#Commands and Protocol) paragraph, but consider that the _Separator Chars_ are no-printable so a message will appears in a serial console like the following:

- Command Sent: `GETkey`
- Response Received:`100value`