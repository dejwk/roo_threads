#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <chrono>
#include <condition_variable>

#include "roo_threads/impl/cppstd/mutex.h"

namespace roo_threads {
/// @brief Backend namespace using C++ standard library synchronization primitives.
namespace cppstd {

/// @ingroup roo_threads_api_condition_variable
/// @copydoc roo_threads::doc::cv_status
enum class cv_status { no_timeout, timeout };

/// @ingroup roo_threads_api_condition_variable
/// @brief C++ standard library backend implementation of
/// `roo::condition_variable`.
/// @copydoc roo_threads::doc::condition_variable
class condition_variable {
 public:
  /// @copydoc roo_threads::doc::condition_variable::condition_variable
  condition_variable() noexcept : cond_() {}

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  /// @copydoc roo_threads::doc::condition_variable::wait
  void wait(unique_lock<mutex>& lock) noexcept { cond_.wait(lock.lock_); }

  /// @copydoc roo_threads::doc::condition_variable::wait
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred) {
    while (!pred()) wait(lock);
  }

  /// @copydoc roo_threads::doc::condition_variable::notify_one
  void notify_one() noexcept { cond_.notify_one(); }

  /// @copydoc roo_threads::doc::condition_variable::notify_all
  void notify_all() noexcept { cond_.notify_all(); }

  /// @copydoc roo_threads::doc::condition_variable::wait_until
  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when) {
    return wait_for(lock, when - roo_time::Uptime::Now());
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_until
  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p) {
    while (!p())
      if (wait_until(lock, when) == cv_status::timeout) return p();
    return true;
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  /// @note Wait duration is clamped to a safe upper bound to avoid overflow.
  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    static constexpr int64_t kMaxSafeWaitMicros = 10LL * 24 * 3600 * 1000000;
    // Protecting against overflow, e.g. wait_for(roo_time::Duration::Max()):
    // never wait for longer than 10 days; spuriously wake up if needed.
    // Using safely low max duration of 10 days, as ESP32 seems to overflow at
    // about 24 days.
    uint64_t micros = duration.inMicros() >= kMaxSafeWaitMicros
                          ? kMaxSafeWaitMicros
                          : duration.inMicros();
    return cond_.wait_for(lock.lock_, std::chrono::microseconds(micros)) ==
                   std::cv_status::no_timeout
               ? cv_status::no_timeout
               : cv_status::timeout;
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  /// @note Wait duration is clamped to a safe upper bound to avoid overflow.
  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p) {
    // Protecting against overflow, e.g. wait_for(roo_time::Duration::Max()):
    // never wait for longer than 10 days; spuriously wake up if needed.
    // Using safely low max duration of 10 days, as ESP32 seems to overflow at
    // about 24 days.
    static constexpr int64_t kMaxSafeWaitMicros = 10LL * 24 * 3600 * 1000000;
    int64_t wait_micros = duration.inMicros() < kMaxSafeWaitMicros
                              ? duration.inMicros()
                              : kMaxSafeWaitMicros;
    return cond_.wait_for(lock.lock_, std::chrono::microseconds(wait_micros),
                          p);
  }

 private:
  std::condition_variable cond_;
};

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD
