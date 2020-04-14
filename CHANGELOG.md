# Changelog

#### v 0.2.1 - 2019.04.14
* Fix: Fixed a bug of timeout in serial comm, it introduces synch error #57
* Fix: Introduced a delay in `begin` to prevent sync error with Arancino Module vers `2.0.0`.

#### v 0.2.0 - 2019.04.09
* Upd: start command send version number, #36
* Upd: keys send a default paramenter #51
* Upd: CI test job #47
* Upd: create new funct seignatures #50

#### v 0.1.2 - 2019.08.30
* Fix: Error during check a reserved key #37

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
