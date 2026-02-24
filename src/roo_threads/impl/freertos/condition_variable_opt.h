#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/cv_status.h"
#include "roo_threads/impl/freertos/mutex.h"
#include "roo_threads/impl/freertos/timeutil.h"

namespace roo_threads {
/// @brief Backend namespace using FreeRTOS synchronization primitives.
namespace freertos {

#ifndef ROO_THREADS_FREERTOS_CONDITION_VARIABLE_MAX_WAITING_THREADS
#define ROO_THREADS_FREERTOS_CONDITION_VARIABLE_MAX_WAITING_THREADS 8
#endif

static constexpr int kMaxWaitingThreads =
    ROO_THREADS_FREERTOS_CONDITION_VARIABLE_MAX_WAITING_THREADS;

/// @ingroup roo_threads_api_condition_variable
/// @brief FreeRTOS optimized implementation of `roo::condition_variable`.
/// @copydoc roo_threads::doc::condition_variable
class condition_variable_opt {
 public:
  /// @copydoc roo_threads::doc::condition_variable::condition_variable
  condition_variable_opt() noexcept;

  condition_variable_opt(const condition_variable_opt&) = delete;
  condition_variable_opt& operator=(const condition_variable_opt&) = delete;

  /// @copydoc roo_threads::doc::condition_variable::wait
  void wait(unique_lock<mutex>& lock) noexcept;

  /// @copydoc roo_threads::doc::condition_variable::wait
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred) {
    while (!pred()) wait(lock);
  }

  /// @copydoc roo_threads::doc::condition_variable::notify_one
  void notify_one() noexcept;

  /// @copydoc roo_threads::doc::condition_variable::notify_all
  void notify_all() noexcept;

  /// @copydoc roo_threads::doc::condition_variable::wait_until
  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when) {
    return wait_until_impl(
        lock, xTaskGetTickCount() +
                  internal::ToTicks(when - roo_time::Uptime::Now()));
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_until
  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p) {
    TickType_t deadline =
        xTaskGetTickCount() + internal::ToTicks(when - roo_time::Uptime::Now());
    while (!p())
      if (wait_until_impl(lock, deadline) == cv_status::timeout) return p();
    return true;
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    return wait_until_impl(lock,
                           xTaskGetTickCount() + internal::ToTicks(duration));
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
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
