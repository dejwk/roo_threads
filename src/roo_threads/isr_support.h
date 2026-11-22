#pragma once

// Resolve ISR support from the runtime independently of ordinary roo
// primitives.
#if defined(ROO_TESTING)
#if defined(ROO_TESTING_RUNTIME_FREERTOS) && ROO_TESTING_RUNTIME_FREERTOS
#define ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#elif defined(ROO_TESTING_RUNTIME_BARE_METAL) &&                         \
    ROO_TESTING_RUNTIME_BARE_METAL && defined(ROO_TESTING_SOC_RP2040) && \
    ROO_TESTING_SOC_RP2040 && defined(PICO_BUILD) && PICO_BUILD
#define ROO_THREADS_INTERNAL_ISR_PICO_CANDIDATE
#endif
#elif defined(ESP32) || defined(ESP_PLATFORM) || \
    (defined(ARDUINO_ARCH_RP2040) && defined(__FREERTOS))
#define ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#elif defined(PICO_ON_DEVICE) && PICO_ON_DEVICE &&            \
    !(defined(LIB_FREERTOS_KERNEL) && LIB_FREERTOS_KERNEL) && \
    (defined(ARDUINO_ARCH_RP2040) ||                          \
     (defined(ROO_THREADS_PICO_SDK_BARE_METAL) &&             \
      ROO_THREADS_PICO_SDK_BARE_METAL))
#define ROO_THREADS_INTERNAL_ISR_PICO_CANDIDATE
#endif

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#include <freertos/FreeRTOS.h>

#if configSUPPORT_STATIC_ALLOCATION == 1
#define ROO_THREADS_INTERNAL_ISR_FREERTOS
#endif
#undef ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#endif

#ifdef ROO_THREADS_INTERNAL_ISR_PICO_CANDIDATE
#include <pico/time.h>

// SDK 2.0.0 timed waits could clear a pending processor event before sleeping.
#if defined(PICO_RP2040) && PICO_RP2040 &&                            \
    (PICO_SDK_VERSION_MAJOR > 2 ||                                    \
     (PICO_SDK_VERSION_MAJOR == 2 && PICO_SDK_VERSION_MINOR >= 1)) && \
    !PICO_TIME_DEFAULT_ALARM_POOL_DISABLED
#define ROO_THREADS_INTERNAL_ISR_PICO
#endif
#undef ROO_THREADS_INTERNAL_ISR_PICO_CANDIDATE
#endif

/// Reports whether this build provides ISR queues, events, and critical
/// sections.
#if defined(ROO_THREADS_INTERNAL_ISR_FREERTOS) || \
    defined(ROO_THREADS_INTERNAL_ISR_PICO)
#define ROO_THREADS_HAS_ISR_SUPPORT 1
#else
#define ROO_THREADS_HAS_ISR_SUPPORT 0
#endif
