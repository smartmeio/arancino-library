## Debug
At this version of the Arancino Library there are two kinds of debug ways independent of each other.

### Serial Debug
This feature was introduced in version [0.0.3](https://git.smartme.io/smartme.io/arancino/arancino-library/milestones/1) and can be enabled in the Arancino Board by setting the `GPIO PA31` - Pin number `26`; after setting the Debug GPIO, power on the Arancino Board (or resetting the microtroller if previously powered on), the Debug data will be sent over the debug serial interface as shown in the table below and you can read it through a serial console, like cu, screen, the Arduino IDE Serial Monitor or the PIO Serial Monitor.

|     Serial    |   SAMD21  |  NRF52  |  STM32  |   Pico  |
|:-------------:|:---------:|:-------:|:-------:|:-------:|
| Communication | SerialUSB |  Serial |  Serial |  Serial |
|     Debug     |   Serial  | Serial1 | Serial1 | Serial1 |

#### Responde Code
Each command sent via the API, will receive a response. Response contains the _Response Code_ and one or more _Response Value_. See [Commands and Protocol](#Commands and Protocol)

#### Debug Messages
Debug messages are similar to those written above in the [Commands and Protocol](#Commands and Protocol) paragraph, but consider that the _Separator Chars_ are no-printable so a message will appears in a serial console like the following:

- Command Sent: `GETkey`
- Response Received:`100value`

### Pub/Sub Debug
This kind of debug uses the potential of the Redis Pub/Sub and it's conveyed by the Daemon's log level. At the begin, Arancino Library retrieves the log level of the Arancino Daemon (via Reseverd Keys) and when it's set to `DEBUG` the `print` and `println` methods will both print the message into the `___MONITOR__` Reseverd Key and publish the message trough a topic with the same name (`__MONITOR___`). 
