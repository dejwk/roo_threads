This library provides concurrency primitives (thread, mutex, condition variable, locks, semaphores, latches, etc.) intended for use on microcontrollers.

They generally mimic C++ standard, with a few small differences for a better fit for embedded devices:
* they are defined in the `roo` namespace,
* they do not depend on `<chrono>`, but on `roo_time`,
* some important thread attributes are supported; specifically, it is possible to set stack size, priority, and name when creating a thread.

Not all of the standard concurrency functionality is ported (yet), but there should be enough here to get you going. Contributions are welcome!

The library has been tested on the ESP32 family, and on Raspberry Pi Pico RP2040 with the FreeRTOS SMP OS. The implementation that uses FreeRTOS primitives is generic, and should be easily portable to other platforms supporting FreeRTOS.

# Basic usage

The minimalistic Arduino program is as follows:

```cpp
#include "Arduino.h"

#include "roo_threads.h"

void setup() {
  Serial.begin(115200);
}

void loop() {
  roo::thread t([]() {
    Serial.println("Hello from roo::thread!");
  });
  t.join();
  delay(1000);
}
```

For esp-idf:

```cpp
#include <cstdio>

#include "roo_threads.h"

extern "C" void app_main() {
  while (true) {
    roo::thread t([]() { printf("Hello from roo::thread!\n"); });
    t.join();
    roo::this_thread::sleep_for(roo_time::Millis(1000));
  }
}
```

For more details, see attached examples.

## ESP32 family

No special configuration is required; the library works out-of-the-box.

The library can be mixed with the concurrency utilities from the standard library (e.g. <atomic>, <mutex>, etc.), and with native FreeRTOS APIs (e.g. xTaskCreate, etc.), since it uses the same underlying primitives.

## Rasbperry Pi Pico

You must install the FreeRTOS OS in order to be able to use `roo_threads`.

For Arduino IDE, see [this video](https://www.youtube.com/watch?v=JfVnUlGTBi8).

For VS Code with pioarduino, use these settings in the `platformio.ini`:

```
[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower

build_flags =
    -D __FREERTOS=1

```

The library can be mixed with the native FreeRTOS APIs (e.g. xTaskCreate, etc.), since it uses the same underlying primitives.

## Compatibility with single-threaded platforms

As a fallback, the library can be compiled in a 'single-threaded' mode, in which the functionality to start new threads is not available, but a lot of mutex/lock functionality
is implemented as a no-op. It allows you to implement thread-safe libraries that still compile and work on single-threaded platforms.

## Testing

The library should work correctly in emulators that support FreeRTOS. In particular, it works correctly with the [http://github.com/dejwk/roo_testing](roo_testing) environment, allowing to unit-test and emulate multi-threaded ESP32 programs on Linux.
