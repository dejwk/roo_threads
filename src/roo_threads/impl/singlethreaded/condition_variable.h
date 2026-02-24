#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

#include "roo_threads/impl/singlethreaded/mutex.h"

namespace roo_threads {
/// @brief Backend namespace for single-threaded synchronization fallbacks.
namespace singlethreaded {

/// @ingroup roo_threads_api_condition_variable
/// @copydoc roo_threads::doc::cv_status
enum class cv_status { no_timeout, timeout };

/// @ingroup roo_threads_api_condition_variable
/// @brief Single-threaded backend implementation of `roo::condition_variable`.
/// @copydoc roo_threads::doc::condition_variable
class condition_variable {
 public:
  /// @copydoc roo_threads::doc::condition_variable::condition_variable
  condition_variable() noexcept {}

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  // Untimed wait is left intentionally unimplemented, as it would block
  // indefinitely.
  /// @copydoc roo_threads::doc::condition_variable::wait
  /// @note Not available in single-threaded mode.
  void wait(unique_lock<mutex>& lock) noexcept;

  // Untimed wait is left intentionally unimplemented, as it would block
  // indefinitely.
  /// @copydoc roo_threads::doc::condition_variable::wait
  /// @note Not available in single-threaded mode.
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred);

  /// @copydoc roo_threads::doc::condition_variable::notify_one
  void notify_one() noexcept {}

  /// @copydoc roo_threads::doc::condition_variable::notify_all
  void notify_all() noexcept {}

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
  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    roo_time::Delay(duration);
    return cv_status::timeout;
  }

  /// @copydoc roo_threads::doc::condition_variable::wait_for
  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p) {
    return wait_until(lock, roo_time::Uptime::Now() + duration, p);
  }
};

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_SINGLETHREADED
