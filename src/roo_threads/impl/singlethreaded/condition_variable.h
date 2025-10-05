#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

#include "roo_threads/impl/singlethreaded/mutex.h"

namespace roo_threads {
namespace singlethreaded {

enum class cv_status { no_timeout, timeout };

class condition_variable {
 public:
  condition_variable() noexcept {}

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  // Untimed wait is left intentionally unimplemented, as it would block
  // indefinitely.
  void wait(unique_lock<mutex>& lock) noexcept;

  // Untimed wait is left intentionally unimplemented, as it would block
  // indefinitely.
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred);

  void notify_one() noexcept {}

  void notify_all() noexcept {}

  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when) {
    return wait_for(lock, when - roo_time::Uptime::Now());
  }

  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p) {
    while (!p())
      if (wait_until(lock, when) == cv_status::timeout) return p();
    return true;
  }

  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration) {
    roo_time::Delay(duration);
    return cv_status::timeout;
  }

  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p) {
    return wait_until(lock, roo_time::Uptime::Now() + duration, p);
  }
};

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_SINGLETHREADED
