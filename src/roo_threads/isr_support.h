#pragma once

// Resolve ISR support from the runtime independently of ordinary roo
// primitives.
#if defined(ROO_TESTING)
#if defined(ROO_TESTING_RUNTIME_FREERTOS) && ROO_TESTING_RUNTIME_FREERTOS
#define ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#endif
#elif defined(ESP32) || defined(ESP_PLATFORM) || \
    (defined(ARDUINO_ARCH_RP2040) && defined(__FREERTOS))
#define ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#endif

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#include <freertos/FreeRTOS.h>

#if configSUPPORT_STATIC_ALLOCATION == 1
#define ROO_THREADS_INTERNAL_ISR_FREERTOS
#endif
#undef ROO_THREADS_INTERNAL_ISR_FREERTOS_CANDIDATE
#endif

/// Reports whether this build provides roo::isr_queue and roo::isr_event.
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
#define ROO_THREADS_HAS_ISR_SUPPORT 1
#else
#define ROO_THREADS_HAS_ISR_SUPPORT 0
#endif
