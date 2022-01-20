## Reserved Keys

**PAY ATTENTION: NOT EVERYTHING DESCRIBED BELLOW IN THIS PARAGRAPH IS YET AVAILABLE**

Arancino Library uses some reserved keys to store environmental data into Redis. Values stored in those keys can be used for multiple scopes in user space. By default the Arancino Library uses those reserved keys to communicate in a synchronous way but it can do also in an asynchronous way.

Following the list of reserved keys:

| Reserved Key/Channel    | Version | Description    |
| ----------------------- | ------- | -------------- |
| `___MONITOR___`             | >=v0.1.0| Used by `print` and `println` API |
| `___MODVERS___`           | * | Used to get the running version number of the Arancino Daemon | 
| `___MODLOGLVL___`           | >=v2.0.0| Actual Log level of the Arancino Deamon|
| `___MODENV___`            | >=2.0.0.| Used to get the running environment of Arancino Daemon|
| `___BLINK_ID___`          | >=v2.0.0| Used by DeviceIdentification of ArancinoTasks for identify the microcontroller|


It is not possible to access directly data stored at Reserved Keys. There are generic private encapsulators (`getReserved`, `setReserved`) only used by the library itself to get and set values. But there are specific method for some Reserved Keys that can be used to retrieve Reserved Keys values in the user space:

- `getModuleVersion()`
- `getModuleLogLevel()`
- `getModuleEnvironment()`

see [API](API.MD) chapter for more details










To access data stored at reserved keys you have to use the Redis `get` command for _Sync_ mode and Redis `Subscribe` for Asynch mode (in _Async_ mode the _key_ name represents the _channel_ name).

By default the Arancino Library uses the _Sync_ mode, but you can change it to work in _Async_ mode or _Both_ (_Sync_ and _Async_ at the same time). To do that you have to define a variable called `RSVD_COMM` at the beginning of your program, with three possible values:

- `0` →  _Sync_
- `1` →  _Async_
- `2` →  _Both_

Example:
```c++
#include <Arancino.h>

void setup() {
  Arancino.begin();
  Arancino.setReservedCommunicationMode(BOTH);
}

void loop() {
  //do something
}

```
