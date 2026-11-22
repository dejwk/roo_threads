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

## Raspberry Pi Pico

On RP2040 boards using Arduino-Pico, enable FreeRTOS SMP to create threads.
The core defines `__FREERTOS` when FreeRTOS is enabled, and `roo_threads`
selects its FreeRTOS backend. With FreeRTOS disabled, the library selects
the single-threaded backend. Define `ROO_THREADS_SINGLETHREADED` to select
that backend explicitly.

In the Arduino IDE, include `<FreeRTOS.h>` in your sketch and select
**Tools → Operating System → FreeRTOS SMP**.
[Arduino-Pico FreeRTOS documentation](https://arduino-pico.readthedocs.io/en/stable/freertos.html#enabling-freertos)

In PlatformIO, enable FreeRTOS in `platformio.ini`.

```ini
[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower
build_flags = -DPIO_FRAMEWORK_ARDUINO_ENABLE_FREERTOS
```

The FreeRTOS backend can be used alongside native FreeRTOS APIs such as
`xTaskCreate`.

## Compatibility with single-threaded platforms

As a fallback, the library can be compiled in a 'single-threaded' mode, in which the functionality to start new threads is not available, but a lot of mutex/lock functionality
is mocked as a no-op. It allows you to implement thread-safe libraries that still compile and work on single-threaded platforms.

## Testing

Use `roo_testing` to emulate and unit-test multithreaded ESP32 programs on
Linux. [roo_testing repository](https://github.com/dejwk/roo_testing)

## Host emulation

Host builds support nine profiles covering Arduino and ESP-IDF on ESP32,
ESP32-S3, and ESP32-C3, plus Pico SDK and Arduino-Pico with and without
FreeRTOS. CI uses the shared workflow from `roo_testing` 2.6. With Bazelisk
1.21 or newer, a plain command defaults to Arduino on ESP32 and prints a
notice.

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

The Pico SDK and bare-metal Arduino-Pico profiles select the
single-threaded backend through `ROO_TESTING_RUNTIME_BARE_METAL`. This backend
provides locks and sleep. Thread-dependent examples and tests are excluded
from these profiles. `backend_selection_test` covers backend selection on
Pico and ESP32.

### Arduino-Pico FreeRTOS emulation

Run the single-core FreeRTOS profile with
`bazel test ... --config=roo_testing_arduino_rp2040_freertos`.
`rp2040_freertos_selection_probe` checks at compile time that this runtime
selects the FreeRTOS backend. Integration tests use automatic and manual
clock modes to verify delays, joins, semaphore handoffs, and
condition-variable timeouts. Hardware core 1 and task affinity are outside
the emulated profile.
