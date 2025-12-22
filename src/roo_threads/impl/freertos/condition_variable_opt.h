#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/cv_status.h"
#include "roo_threads/impl/freertos/mutex.h"
#include "roo_threads/impl/freertos/timeutil.h"

static constexpr int kMaxWaitingThreads = 8;

namespace roo_threads {
namespace freertos {

class condition_variable_opt {
 public:
  condition_variable_opt() noexcept;

  condition_variable_opt(const condition_variable_opt&) = delete;
  condition_variable_opt& operator=(const condition_variable_opt&) = delete;

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

#endif  // ROO_THREADS_USE_FREERTOS
