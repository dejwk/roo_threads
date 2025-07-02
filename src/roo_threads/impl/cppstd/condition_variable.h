#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include "roo_threads/impl/cppstd/mutex.h"

#include <chrono>
#include <condition_variable>

namespace roo_threads {
namespace cppstd {

enum class cv_status { no_timeout, timeout };

class condition_variable {
 public:
  condition_variable() noexcept : cond_() {}

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  void wait(unique_lock<mutex>& lock) noexcept { cond_.wait(lock.lock_); }

  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred) {
    while (!pred()) wait(lock);
  }

  void notify_one() noexcept { cond_.notify_one(); }

  void notify_all() noexcept { cond_.notify_all(); }

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
                     const roo_time::Interval& duration) {
    return cond_.wait_for(lock.lock_,
                          std::chrono::microseconds(duration.inMicros())) ==
                   std::cv_status::no_timeout
               ? cv_status::no_timeout
               : cv_status::timeout;
  }

  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Interval& duration,
                Predicate p) {
    return cond_.wait_for(lock.lock_,
                          std::chrono::microseconds(duration.inMicros()), p);
  }

 private:
  std::condition_variable cond_;
};

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD
