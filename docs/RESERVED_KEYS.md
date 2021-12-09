## Reserved Keys

**PAY ATTENTION: NOT EVERYTHING DESCRIBED BELLOW IN THIS PARAGRAPH IS YET AVAILABLE**

Arancino Library uses some reserved keys to store environmental data into Redis. Values stored in those keys can be used for multiple scopes in user space. By default the Arancino Library uses those reserved keys to communicate in a synchronous way but it can do also in an asynchronous way.

Following the list of reserved keys:

| Reserved Key/Channel    | Version | Description    |
| ----------------------- | ------- | -------------- |
| `___POWER___`           | N/A     | The value can be `battery` or `power`, based on the power supply type. Very useful e.g. when switching from power to battery and automatically softly shoutdown the system. It works only with the specific hardware module for the Arancino Board. (Not yet implemented)|
| `___MONITOR___`         | >=v0.1.0| used by `print` and `println` API |
| `__LIBVERS___`          | >=v0.1.0| Arancino Library version (No more used) |


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
