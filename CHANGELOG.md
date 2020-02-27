# Changelog

#### v 1.1.0 - 2020.MM.DD
* Fix: CI script to push even master branch to GitHub
* Upd: `KEYS` Command sends to Arancino Module a default arg (`*`) when call `Arancino.keys()`
* Add: Introduced new signatures with `long` type for function: `set`, `hset` and `publish`
* Add: Introduced `Test` Job in CI to test every Arancino examples.
* Upd: Improved API wrapping.

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
