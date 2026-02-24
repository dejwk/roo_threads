#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <limits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/cv_status.h"
#include "roo_threads/impl/freertos/mutex.h"
#include "roo_threads/impl/freertos/semaphore.h"
#include "roo_threads/impl/freertos/timeutil.h"

static constexpr int kMaxWaitingThreads = 8;

namespace roo_threads {
/// @brief Backend namespace using FreeRTOS synchronization primitives.
namespace freertos {

/// @ingroup roo_threads_api_condition_variable
/// @brief FreeRTOS classic implementation of `roo::condition_variable`.
/// @copydoc roo_threads::doc::condition_variable
class condition_variable_classic {
 public:
  /// @copydoc roo_threads::doc::condition_variable::condition_variable
  condition_variable_classic() noexcept;

  condition_variable_classic(const condition_variable_classic&) = delete;
  condition_variable_classic& operator=(const condition_variable_classic&) =
      delete;

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
  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when);

  /// @copydoc roo_threads::doc::condition_variable::wait_until
  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p) {
    while (!p())
      if (wait_until(lock, when) == cv_status::timeout) return p();
    return true;
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    return wait_until(lock, internal::CalculateDeadlineFromDuration(duration));
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p) {
    return wait_until(lock, internal::CalculateDeadlineFromDuration(duration),
                      std::move(p));
  }

 private:
  mutex mutex_;
  counting_semaphore<std::numeric_limits<BaseType_t>::max()> waitlist_;
  int waitcount_;
};

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
