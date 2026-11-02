// Isolate physical-core macro selection from the host test profile.
#undef ROO_TESTING
#undef ROO_THREADS_SINGLETHREADED
#undef ROO_THREADS_USE_FREERTOS
#undef ROO_THREADS_USE_CPPSTD
#undef ESP32
#undef ESP_PLATFORM
#undef __FREERTOS
#undef ARDUINO_ARCH_RP2040
#define ARDUINO_ARCH_RP2040 1
#if TEST_PICO_FREERTOS
#define __FREERTOS 1
#endif
#if TEST_PICO_OVERRIDE
#define ROO_THREADS_SINGLETHREADED
#endif
#include "roo_threads/impl/resolve.h"
#if TEST_PICO_FREERTOS && !TEST_PICO_OVERRIDE
#ifndef ROO_THREADS_USE_FREERTOS
#error Physical FreeRTOS Pico selected the wrong backend
#endif
#ifdef ROO_THREADS_SINGLETHREADED
#error Physical FreeRTOS Pico selected single-threaded support
#endif
#else
#ifndef ROO_THREADS_SINGLETHREADED
#error Physical bare-metal/explicit override selected the wrong backend
#endif
#ifdef ROO_THREADS_USE_FREERTOS
#error Unexpected FreeRTOS backend
#endif
#endif
// Verifies physical core selection and the explicit single-threaded override.
int main() { return 0; }
