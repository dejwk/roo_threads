#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace roo_threads {
namespace internal {

// ESP-IDF ports accept a no-argument request. The Pico port takes the flag.
inline void YieldFromIsr(BaseType_t task_woken) noexcept {
#ifdef portMUX_INITIALIZER_UNLOCKED
  if (task_woken == pdTRUE) portYIELD_FROM_ISR();
#else
  portYIELD_FROM_ISR(task_woken);
#endif
}

}  // namespace internal
}  // namespace roo_threads
