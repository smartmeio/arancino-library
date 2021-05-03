# Changelog

#### v 2.0.0 - 2021.05.03
* Upd: removed `metadata` function. [fb6hyw] (https://app.clickup.com/t/fb6hyw)
* Upd: removed check on reserved command. [fv2dmy] (https://app.clickup.com/t/fv2dmy)
* Upd: updated sync/async mode. [fd59jj] (https://app.clickup.com/t/fd59jj)
* Add: added persistent mode to `mset` and `hset` function. [aex4by] (https://app.clickup.com/t/aex4by)
* Add: added `getReserved` function. [g31n4z](https://app.clickup.com/t/g31n4z)
* Add: added `store` function. [#gk4cn8](https://app.clickup.com/t/gk4cn8)
* Add: added `storetags` function. [#hf2fg6](https://app.clickup.com/t/hf2fg6)

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
