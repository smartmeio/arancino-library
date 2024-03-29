#  Arancino Library

The following Library works on Arancino.cc boards, it is written in Arduino lang and can be imported into the Arduino IDE and Platformio.IO. Arancino Library was written originally to run on [SAMD21](https://github.com/smartmeio/arancino-core-samd21) Microcontroller, now it can runs also on [NRF52](https://github.com/smartmeio/arancino-core-nrf52), [STM32](https://github.com/smartmeio/arancino-core-stm32) and [RP2040](https://github.com/smartmeio/arancino-core-rp2040) Microcontrollers. It uses the serial connection to communicate with the [Arancino Daemon](https://github.com/smartmeio/arancino-daemon) which runs on the linux side of the Arancino.cc board. It uses a serial interface to communicate with Arancino Daemon and another Serial for Debug (see more on [Debug](docs/DEBUG.md) chapter)

  

Arancino Library allows to export/import data to/from the Linux environment using Redis as database cache. Most of the API are modelled on Redis standard Commands.

  

-  [Getting Started](docs/GETTING_STARTED.md)

-  [Variables and Data Structures](docs/VARS_DATA_STRUCT.md)

-  [API](docs/API.md)

-  [FreeRTOS](docs/FREERTOS.md)

-  [Cortex Protocol](docs/CORTEX.md)

-  [Debug](docs/DEBUG.md)

-  [Reserved Keys](docs/RESERVED_KEYS.md)

##  Compatibility list
The Arancino Library `3.1.1` officially supports the following Arancino Core versions:
|Board| Core Version |
|--|--|
| SAMD21 | 1.4.0 |
| nRF52 | 1.1.1 |
| RP2040 | 1.2.1 |


##  Credits

Most of this documentation has been extrapolated from [Redis Command](https://redis.io/commands/), and [FreeRTOS quick start guide](https://www.freertos.org/FreeRTOS-quick-start-guide.html).

  

Redis, Arduino, Atmel, Nordic, STMicroelectronics, RaspberryPi and FreeRTOS are trademarks of their respective owners.
