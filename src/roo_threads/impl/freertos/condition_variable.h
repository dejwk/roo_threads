#pragma once

// #include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/mutex.h"

static constexpr int kMaxWaitingThreads = 8;

namespace roo_threads {
namespace freertos {

enum class cv_status { no_timeout, timeout };

namespace internal {

// Truncate to half of the representation range, to avoid overflow in the
// FreeRTOS API, taking into account that tick counter overflows.
constexpr TickType_t kMaxTicksDelay = 1 << (sizeof(TickType_t) * 8 - 1);

inline constexpr TickType_t ToTicks(roo_time::Duration duration) {
  int64_t micros = duration.inMicros();
  if (micros <= 0) return 0;
  uint64_t ms = (micros + 999) / 1000;
  uint64_t ticks = (ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;
  return (ticks <= kMaxTicksDelay) ? static_cast<TickType_t>(ticks)
                                   : kMaxTicksDelay;
}

}  // namespace internal

class condition_variable {
 public:
  condition_variable() noexcept;

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  void wait(unique_lock<mutex>& lock) noexcept;

  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred) {
    while (!pred()) wait(lock);
  }

  void notify_one() noexcept;

  void notify_all() noexcept;

  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when) {
    return wait_until_impl(
        lock, xTaskGetTickCount() +
                  internal::ToTicks(when - roo_time::Uptime::Now()));
  }

  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p) {
    TickType_t deadline =
        xTaskGetTickCount() + internal::ToTicks(when - roo_time::Uptime::Now());
    while (!p())
      if (wait_until_impl(lock, deadline) == cv_status::timeout) return p();
    return true;
  }

  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    return wait_until_impl(lock,
                           xTaskGetTickCount() + internal::ToTicks(duration));
  }

  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p) {
    TickType_t deadline = xTaskGetTickCount() + internal::ToTicks(duration);
    while (!p()) {
      if (wait_until_impl(lock, deadline) == cv_status::timeout) return p();
    }
    return true;
  }

 private:
  cv_status wait_until_impl(unique_lock<mutex>& lock, TickType_t when) noexcept;

  TaskHandle_t tasks_waiting_[kMaxWaitingThreads];
};

}  // namespace freertos
}  // namespace roo_threads
