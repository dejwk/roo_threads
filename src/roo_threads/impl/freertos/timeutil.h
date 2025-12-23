#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/mutex.h"
#include "roo_time.h"

namespace roo_threads {
namespace freertos {

namespace internal {

// Truncate to half of the representation range, to avoid overflow in the
// FreeRTOS API, taking into account that tick counter overflows.
constexpr TickType_t kMaxTicksDelay = portMAX_DELAY / 2;

inline constexpr TickType_t ToTicks(roo_time::Duration duration) {
  int64_t micros = duration.inMicros();
  if (micros <= 0) return 0;
  uint64_t ms = (micros + 999) / 1000;
  uint64_t ticks = (ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;
  return (ticks <= kMaxTicksDelay) ? static_cast<TickType_t>(ticks)
                                   : kMaxTicksDelay;
}

inline roo_time::Uptime CalculateDeadlineFromDuration(
    const roo_time::Duration& duration) {
  auto now = roo_time::Uptime::Now();
  return (duration <= roo_time::Uptime::Max() - now) ? now + duration
                                                     : roo_time::Uptime::Max();
}

}  // namespace internal

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
