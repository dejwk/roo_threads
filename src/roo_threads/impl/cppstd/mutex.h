#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <chrono>
#include <mutex>

namespace roo_threads {
/// @brief Backend namespace using C++ standard library synchronization primitives.
namespace cppstd {

/// @ingroup roo_threads_api_mutex
/// @brief C++ standard library backend implementation of `roo::mutex`.
/// @copydoc roo_threads::doc::mutex
using mutex = std::mutex;

/// @ingroup roo_threads_api_mutex
/// @brief C++ standard library backend implementation of `roo::lock_guard`.
/// @copydoc roo_threads::doc::lock_guard
template <typename Mutex>
using lock_guard = ::std::lock_guard<Mutex>;

/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::defer_lock_t
using defer_lock_t = std::defer_lock_t;
/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::try_to_lock_t
using try_to_lock_t = std::try_to_lock_t;
/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::adopt_lock_t
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
/// @ingroup roo_threads_api_mutex
class unique_lock {
 public:
  typedef Mutex mutex_type;

  /// @copydoc roo_threads::doc::unique_lock::unique_lock() noexcept
  unique_lock() noexcept : lock_() {}

  /// @copydoc roo_threads::doc::unique_lock::unique_lock(mutex_type&)
  explicit unique_lock(mutex_type& mutex) : lock_(mutex) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,defer_lock_t)
  unique_lock(mutex_type& mutex, defer_lock_t) noexcept
      : lock_(mutex, std::defer_lock) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,try_to_lock_t)
  unique_lock(mutex_type& mutex, try_to_lock_t)
      : lock_(mutex, std::try_to_lock) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,adopt_lock_t)
  unique_lock(mutex_type& mutex, adopt_lock_t) noexcept
      : lock_(mutex, std::adopt_lock) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,roo_time::Uptime)
  unique_lock(mutex_type& mutex, roo_time::Uptime tp)
      : lock_(mutex, tp - roo_time::Uptime::Now()) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,roo_time::Duration)
  unique_lock(mutex_type& mutex, roo_time::Duration duration)
      : lock_(mutex, std::chrono::microseconds(duration.inMicros())) {}

  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;

  /// @copydoc roo_threads::doc::unique_lock::unique_lock(unique_lock&&)
  unique_lock(unique_lock&& lock) noexcept : lock_(std::move(lock.lock_)) {}

  /// @copydoc roo_threads::doc::unique_lock::operator=(unique_lock&&)
  unique_lock& operator=(unique_lock&& lock) noexcept {
    lock_ = std::move(lock.lock_);
    return *this;
  }

  /// @copydoc roo_threads::doc::unique_lock::lock
  void lock() { lock_.lock(); }

  /// @copydoc roo_threads::doc::unique_lock::try_lock
  bool try_lock() { return lock_.try_lock(); }

  /// @copydoc roo_threads::doc::unique_lock::try_lock_until
  bool try_lock_until(roo_time::Uptime tp) {
    return try_lock_for(tp - roo_time::Uptime::Now());
  }

  /// @copydoc roo_threads::doc::unique_lock::try_lock_for
  bool try_lock_for(roo_time::Duration duration) {
    return lock_.try_lock_for(std::chrono::microseconds(duration.inMicros()));
  }

  /// @copydoc roo_threads::doc::unique_lock::unlock
  void unlock() { lock_.unlock(); }

  /// @copydoc roo_threads::doc::unique_lock::swap
  void swap(unique_lock& other) noexcept { lock_.swap(other.lock_); }

  /// @copydoc roo_threads::doc::unique_lock::release
  mutex_type* release() noexcept { return lock_.release(); }

  /// @copydoc roo_threads::doc::unique_lock::owns_lock
  bool owns_lock() const noexcept { return lock_.owns_lock(); }

  /// @copydoc roo_threads::doc::unique_lock::operator bool
  explicit operator bool() const noexcept { return lock_; }

  /// @copydoc roo_threads::doc::unique_lock::mutex
  mutex_type* mutex() const noexcept { return lock_.mutex(); }

 private:
  friend class condition_variable;

  std::unique_lock<Mutex> lock_;
};

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD