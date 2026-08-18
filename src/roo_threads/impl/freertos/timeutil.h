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

// Truncate to half of the representation range so tick-counter arithmetic is
// unambiguous. Also apply the same ten-day portable ceiling as the cppstd
// backend: the Linux FreeRTOS port uses a 64-bit TickType_t, but its underlying
// timed-wait APIs cannot represent a half-range delay without overflowing.
constexpr uint64_t kTenDaysInTicks =
    (10ULL * 24 * 60 * 60 * 1000) / portTICK_PERIOD_MS;
constexpr TickType_t kHalfRangeMaxTicksDelay = portMAX_DELAY / 2;
constexpr TickType_t kMaxTicksDelay =
    static_cast<uint64_t>(kHalfRangeMaxTicksDelay) < kTenDaysInTicks
        ? kHalfRangeMaxTicksDelay
        : static_cast<TickType_t>(kTenDaysInTicks);

inline constexpr TickType_t ToTicks(roo_time::Duration duration) {
  int64_t micros = duration.inMicros();
  if (micros <= 0) return 0;
  constexpr uint64_t kMaxSafeWaitMicros =
      static_cast<uint64_t>(kMaxTicksDelay) * portTICK_PERIOD_MS * 1000;
  if (static_cast<uint64_t>(micros) >= kMaxSafeWaitMicros) {
    return kMaxTicksDelay;
  }
  uint64_t ms = (static_cast<uint64_t>(micros) + 999) / 1000;
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
