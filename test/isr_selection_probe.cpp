// Isolate the physical Pico capability selector from the host profile.
#undef ROO_TESTING
#undef ESP32
#undef ESP_PLATFORM
#undef __FREERTOS
#undef ARDUINO_ARCH_RP2040
#undef PICO_ON_DEVICE
#undef PICO_RP2040
#undef PICO_RP2350
#undef PICO_SDK_VERSION_MAJOR
#undef PICO_SDK_VERSION_MINOR
#undef PICO_TIME_DEFAULT_ALARM_POOL_DISABLED
#undef LIB_FREERTOS_KERNEL

#define PICO_ON_DEVICE 1
#define PICO_RP2040 TEST_RP2040
#define PICO_RP2350 (!TEST_RP2040)
#define PICO_SDK_VERSION_MAJOR 2
#define PICO_SDK_VERSION_MINOR TEST_SDK_MINOR
#define PICO_TIME_DEFAULT_ALARM_POOL_DISABLED TEST_ALARM_POOL_DISABLED
#define LIB_FREERTOS_KERNEL TEST_FREERTOS
#define ROO_THREADS_PICO_SDK_BARE_METAL TEST_BARE_METAL

#include "roo_threads/isr_support.h"

static_assert(ROO_THREADS_HAS_ISR_SUPPORT == TEST_ISR_SUPPORT,
              "Unexpected Pico ISR capability");

// Verifies the preprocessor selected the expected ISR capability.
int main() { return 0; }
