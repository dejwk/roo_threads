This library provides concurrency primitives (thread, mutex, condition variable, locks, semaphores, latches, etc.) intended for use on microcontrollers.

The library has been tested on the ESP32 family (under Arduino as well as raw esp-idf), and on Raspberry Pi Pico RP2040 with the FreeRTOS SMP OS. The FreeRTOS-based implementation is generic, and should be easily portable to other platforms supporting FreeRTOS.

The code mimics the C++ standard concurrency library, with a few small differences, mainly to better support embedded devices:
* the classes are defined in the `roo` namespace,
* they do not depend on `<chrono>`, but on `roo_time`,
* some useful thread attributes are directly supported; specifically, it is possible to set stack size, priority, and name when creating a thread.

Not all of the standard concurrency functionality is ported (yet), but there should be enough here to get you going. Contributions are welcome!

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

The library can be mixed with the concurrency utilities from the standard library (e.g. `<atomic>`, `<mutex>`, etc.), and with native FreeRTOS APIs (e.g. `xTaskCreate`, etc.), since it uses the same underlying primitives.

## Rasbperry Pi Pico

You must install the FreeRTOS OS in order to be able to use this library.

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
is mocked as a no-op. It allows you to implement thread-safe libraries that still compile and work on single-threaded platforms.

## Testing

The library should work correctly in emulators that support FreeRTOS. In particular, it works correctly with the [http://github.com/dejwk/roo_testing](roo_testing) environment, allowing to unit-test and emulate multi-threaded ESP32 programs on Linux.

## Host emulation

Host builds support both Arduino and ESP-IDF through roo_testing 2.0. With
Bazelisk 1.21 or newer, a plain command defaults to Arduino and prints a notice:

    bazel test ...
    bazel test ... --config=asan
    bazel test ... --config=roo_testing_arduino_esp32
    bazel test ... --config=roo_testing_idf_esp32
    .roo_testing/bin/test_all_profiles ...

The files under .roo_testing are vendored from roo_testing; follow their
canonical-source headers when refreshing them.

Arduino examples are native runnable targets in their source packages. For
example:

    bazel run //examples/Counter

### Bare-metal host emulation

Pico host profiles select the existing single-threaded backend through
`ROO_TESTING_RUNTIME_BARE_METAL`, before the general emulation FreeRTOS branch.
This selection follows the runtime, not the chip: future RP2040 FreeRTOS host
profiles retain the FreeRTOS backend. Existing ESP32 profiles are unchanged.
The single-threaded backend supplies locks and sleep but cannot create, join
or detach threads, wait for another thread to signal a condition, or yield to
a scheduler. Thread-dependent examples and tests are excluded from this runtime.

Use a local override of this repository with the matching roo_testing changes
until coordinated releases are available. `backend_selection_test` exercises
the supported public contract in Pico and ESP32 profiles;
`rp2040_freertos_selection_probe` is compile-only evidence that Pico identity
does not override a FreeRTOS runtime, and does not emulate Pico FreeRTOS.


Arduino-Pico emulation also supports the real single-core FreeRTOS backend:
`bazel test ... --config=roo_testing_arduino_rp2040_freertos`.
Use the matching roo_testing revision that provides this profile. The dedicated
auto/manual integration tests verify Roo delays, joins, semaphore handoffs, and
condition-variable timeouts against the shared emulated clock. Hardware core 1
and task affinity are outside the emulated profile.
