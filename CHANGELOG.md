# Changelog

#### v 3.1.0 - 2023.x.x
* Upd: Implemented new Cortex protocol [#1jdx7ak] (https://app.clickup.com/t/1jdx7ak)
* Add: Added support to ESP32 [#863gbd6ym] (https://app.clickup.com/t/863gbd6ym)
* Add: Added support to NRF52 [#863gbd74b] (https://app.clickup.com/t/863gbd74b)
* Upd: Increased Serial timeout [#33kn3c2] (https://app.clickup.com/t/33kn3c2)
* Upd: Renamed task GetBlinkId [#31ewyzv] (https://app.clickup.com/t/31ewyzv)
* Upd: Updated examples [#23n7teq] (https://app.clickup.com/t/23n7teq)
* Upd: Manage new DB instance for configuration keys [#23fqct7] (https://app.clickup.com/t/23fqct7)
* Upd: Introduced ACK in `SET` methods [#23fpyp2] (https://app.clickup.com/t/23fpyp2)
* Upd: Service functions in a single task [#861mjudeh] (https://app.clickup.com/t/861mjudeh)
* Upd: Ifaces adapted to new protocol [#861mjue8p] (https://app.clickup.com/t/861mjue8p)
* Upd: Fixed warnings in `const char*` conversion [#863gbd62q] (https://app.clickup.com/t/863gbd62q)

#### v 3.0.0 - 2022.10.10
* Add: added interface system with MQTT and BLE_UART communication mediums [#1nwtwna]
* Upd: improved debug messages handling [#1ww0cnr]
* Add: added Arancino delay function [#n1c5tx]
* Add: Added hearbeat function as additional task [#1raryn7]

#### v 2.1.1 - 2022.09.19
* Fix: fixed an error that occurred when FreeRTOS was not enabled
* Fix: fixed a bug in executeCommandFast function
* Fix: fixed an error in STORE_packet example

#### v 2.1.0 - 2022.07.29
* Upd: ERR_GENERIC was redefined as ERR [#1jz91mu] (https://app.clickup.com/t/1jz91mu)
* Upd: ID_PREFIX removed from `store`, `mstore` and `storeTags` commands [#23v2xn3] (https://app.clickup.com/t/23v2xn3)
* Add: added executeCommandFast command [#394mek4] (https://app.clickup.com/t/394mek4)
* Add: added `mstore` with timestamp argument [#394memb] (https://app.clickup.com/t/394memb)

#### v 2.0.1 - 2022.05.27
* Fix: bug debug for arancino mignon board. [#2g4fxvx] (https://app.clickup.com/t/2g4fxvx)

#### v 2.0.0 - 2022.01.17
* Upd: updated unit test examples. [#1uqbjbp] (https://app.clickup.com/t/1uqbjbp)
* Add: added useFreeRtos parameter in `start` command. [#1vht0nc] (https://app.clickup.com/t/1vht0nc)
* Fix: bug into `deviceIdentification` task. [#1w8qx9m] (https://app.clickup.com/t/1w8qx9m)
* Add: added examples wthout freeRtos. [#1vhrhj2] (https://app.clickup.com/t/1vhrhj2)
* Fix: read internal temperature for STM32. [#pbbnm1] (https://app.clickup.com/t/pbbnm1)
* Udp: code revision. [#j58vz6] (https://app.clickup.com/t/j58vz6)
* Udp: examples revision. [#jd3yjq] (https://app.clickup.com/t/jd3yjq)
* Add: read internal temperature for STM32. [#pbbnm1] (https://app.clickup.com/t/pbbnm1)
* Add: Arduino `loop` as FreeRtos task. [#p59vhb](https://app.clickup.com/t/p59vhb)
* Fix: fixed bug with float and double using FreeRtos. [#n990qu](https://app.clickup.com/t/n990qu)
* Add: added interocep function. [#d2x5yt](https://app.clickup.com/t/d2x5yt)
* Add: added `setReserved` function. [#n1amcu](https://app.clickup.com/t/n1amcu)
* Upd: removed deprecated arguments on the `begin` function. [fb6jve] (https://app.clickup.com/t/fb6jve)
* Upd: removed `metadata` function. [fb6hyw] (https://app.clickup.com/t/fb6hyw)
* Upd: removed check on reserved command. [fv2dmy] (https://app.clickup.com/t/fv2dmy)
* Upd: updated sync/async mode. [fd59jj] (https://app.clickup.com/t/fd59jj)
* Add: added persistent mode to `mset` and `hset` function. [aex4by] (https://app.clickup.com/t/aex4by)
* Add: added `getReserved` function. [g31n4z](https://app.clickup.com/t/g31n4z)
* Add: added `store` function. [#gk4cn8](https://app.clickup.com/t/gk4cn8)
* Add: added `storetags` function. [#hf2fg6](https://app.clickup.com/t/hf2fg6)

#### v 1.4.2 - 2021.06.15
* Fix: Fixed bug with `free` function for avr board. [#mhcbn0](https://app.clickup.com/t/mhcbn0)
* Fix: Fixed bug with uint type for avr board. [#mhcbn0](https://app.clickup.com/t/mhcbn0)

#### v 1.4.1 - 2021.04.06
* Fix: Fixed bug with `println` function. [#gk5z1y](https://app.clickup.com/t/gk5z1y)
* Fix: Fixed bug with float's and double's types in `set` function. [#g71qk9](https://app.clickup.com/t/g71qk9)

#### v 1.4.0 - 2021.03.15
* Upd: Metadata structure is now mandatory to the `begin` function. [#aex3e4](https://app.clickup.com/t/aex3e4)
* Add: New `ArancinoConfig` Class used to pass configuration at `begin`. [#fd4ped](https://app.clickup.com/t/fd4ped)

#### v 1.3.1 - 2021.02.08
* Fix: Fixed a memory leak bug's `get`-based functions [#c8y3rg](https://app.clickup.com/t/c8y3rg)
* Add: Utility to use for UTF-8 verification [#cq2620](https://app.clickup.com/t/cq2620)

#### v 1.3.0 - 2020.10.28
* Upd: `get`-type functions now return NULL when key is not found #81
* Add: now it is possible to pass information about firmware name and version to Arancino Module #76
* Add: added `mset` and `mget` functions #74
* Add: added unit tests #83
* Docs: several documentation fixes #82 #61 #60

#### v 1.2.1 - 2020.10.09
* Fix: Fixed a bug in start command #78
* Upd: Updated Doc README for `HGETALL` function #55

#### v 1.2.0 - 2020.07.25
* Fix: Fixed arancino `publish` example  #72
* Fix: Fixed a bug of timeout setting in serial comm  #70
* Add: Introduced the possibility to use prefix id for each command sent #66
* Add: Introduced `arancino.id` attribute #65
* Add: Receive timestamp from Arancino Module #71
* Fix: Fixed a bug of timeout in serial comm, it introduces synch error #57
* Fix: Introduced checks to avoid NULL keys and values #58

#### v 1.1.0 - 2020.03.16
* Fix: CI script to push even master branch to GitHub #52
* Upd: `KEYS` Command sends to Arancino Module a default arg (`*`) when call `Arancino.keys()` #51
* Add: Introduced new signatures with `long` type for function: `set`, `hset` and `publish` #50
* Add: Introduced `Test` Job in CI to test every Arancino examples. #47
* Upd: Improved API wrapping. #41

#### v 1.0.2 - 2020.02.27
* fix: Fixed a regression which occurs whene using `FreeRTOS` with Arancino. #54

#### v 1.0.1 - 2020.01.10
* fix: `publish` API function now always returns an ArancinoPacket.
* fix: memory issues: free is called after variable usage.

#### v 1.0.0 - 2019.12.20
* Integration with FreeRTOS
* Introduced ArancinoPacket to manage returns of Cortex Protocol, error cases and etc..
* Using `char*` instead of `String` to fix some memory issues
* Users can now store persistently values.
* Increased stability and efficiency

#### v 0.1.2 - 2019.08.30
* Hot Fix: Error during check a reserved key

#### v 0.1.1 - 2019.04.17
* Fix: cannot set empty string #26
* Fix: cannot use unint32_t variables #18
* Fix: Rename checkReserverCommand function  #23
* Fix: `print` e `println` does not work #21
* add `setReservedCommunicationMode` API function - Communication Mode for Reserved Keys #17

#### v 0.1.0 - 2019.04.04
* add `print` and `println` API functions.
* Introduce new reserved keys.
* add `flush` API function.
* add `publish` API function.
* Fix: _overload_ for `hset` function.
* add basic examples for API functions.
