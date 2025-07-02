#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <chrono>
#include <mutex>

namespace roo_threads {
namespace cppstd {

using mutex = std::mutex;

template <typename Mutex>
using lock_guard = ::std::lock_guard<Mutex>;

using defer_lock_t = std::defer_lock_t;
using try_to_lock_t = std::try_to_lock_t;
using adopt_lock_t = std::adopt_lock_t;

#if __cplusplus > 201402L
inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};
#else
constexpr defer_lock_t defer_lock{};
constexpr try_to_lock_t try_to_lock{};
constexpr adopt_lock_t adopt_lock{};
#endif

template <typename Mutex>
class unique_lock {
 public:
  typedef Mutex mutex_type;

  unique_lock() noexcept : lock_() {}

  explicit unique_lock(mutex_type& mutex) : lock_(mutex) {}

  unique_lock(mutex_type& mutex, defer_lock_t) noexcept
      : lock_(mutex, std::defer_lock) {}

  unique_lock(mutex_type& mutex, try_to_lock_t)
      : lock_(mutex, std::try_to_lock) {}

  unique_lock(mutex_type& mutex, adopt_lock_t) noexcept
      : lock_(mutex, std::adopt_lock) {}

  unique_lock(mutex_type& mutex, roo_time::Uptime tp)
      : lock_(mutex, tp - roo_time::Uptime::Now()) {}

  unique_lock(mutex_type& mutex, roo_time::Interval duration)
      : lock_(mutex, std::chrono::microseconds(duration.inMicros())) {}

  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;

  unique_lock(unique_lock&& lock) noexcept : lock_(std::move(lock.lock_)) {}

  unique_lock& operator=(unique_lock&& lock) noexcept {
    lock_ = std::move(lock.lock_);
    return *this;
  }

  void lock() { lock_.lock(); }

  bool try_lock() { return lock_.try_lock(); }

  bool try_lock_until(roo_time::Uptime tp) {
    return try_lock_for(tp - roo_time::Uptime::Now());
  }

  bool try_lock_for(roo_time::Interval duration) {
    return lock_.try_lock_for(std::chrono::microseconds(duration.inMicros()));
  }

  void unlock() { lock_.unlock(); }

  void swap(unique_lock& other) noexcept { lock_.swap(other.lock_); }

  mutex_type* release() noexcept { return lock_.release(); }

  bool owns_lock() const noexcept { return lock_.owns_lock(); }

  explicit operator bool() const noexcept { return lock_; }

  mutex_type* mutex() const noexcept { return lock_.mutex(); }

 private:
  friend class condition_variable;

  std::unique_lock<Mutex> lock_;
};

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD