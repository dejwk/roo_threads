#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

#include "roo_backport.h"
#include "roo_backport/move.h"

namespace roo_threads {
/// @brief Backend namespace for single-threaded synchronization fallbacks.
namespace singlethreaded {

/// @ingroup roo_threads_api_mutex
/// @brief Single-threaded backend implementation of `roo::mutex`.
/// @copydoc roo_threads::doc::mutex
class mutex {
 public:
  /// @copydoc roo_threads::doc::mutex::mutex
  mutex() noexcept = default;
  ~mutex() = default;

  mutex(const mutex&) = delete;
  mutex& operator=(const mutex&) = delete;

  /// @copydoc roo_threads::doc::mutex::lock
  void lock() {}
  /// @copydoc roo_threads::doc::mutex::try_lock
  bool try_lock() { return true; }

  /// @copydoc roo_threads::doc::mutex::unlock
  void unlock() {}
};

template <typename Mutex>
/// @ingroup roo_threads_api_mutex
class lock_guard {
 public:
  typedef Mutex mutex_type;

  /// @copydoc roo_threads::doc::lock_guard::lock_guard
  explicit lock_guard(mutex_type& mutex) {}

  lock_guard(const lock_guard&) = delete;
  lock_guard& operator=(const lock_guard&) = delete;
};

// Do not lock the associated mutex.
/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::defer_lock_t
struct defer_lock_t {
  explicit defer_lock_t() = default;
};

// Try to lock the associated mutex without blocking.
/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::try_to_lock_t
struct try_to_lock_t {
  explicit try_to_lock_t() = default;
};

// Assume the calling thread already holds a non-shared lock (i.e., a lock
// acquired by lock, try_lock, try_lock_for, or try_lock_until) on the mutex.
/// @ingroup roo_threads_api_mutex
/// @copydoc roo_threads::doc::adopt_lock_t
struct adopt_lock_t {
  explicit adopt_lock_t() = default;
};

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
  unique_lock() noexcept : lock_(nullptr), owns_(false) {}

  /// @copydoc roo_threads::doc::unique_lock::unique_lock(mutex_type&)
  explicit unique_lock(mutex_type& mutex) : lock_(&mutex), owns_(false) {
    lock();
  }

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,defer_lock_t)
  unique_lock(mutex_type& mutex, defer_lock_t) noexcept
      : lock_(&mutex), owns_(false) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,try_to_lock_t)
  unique_lock(mutex_type& mutex, try_to_lock_t)
      : lock_(&mutex), owns_(lock_->try_lock()) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,adopt_lock_t)
  unique_lock(mutex_type& mutex, adopt_lock_t) noexcept
      : lock_(&mutex), owns_(true) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,roo_time::Uptime)
  unique_lock(mutex_type& mutex, const roo_time::Uptime& tp)
      : lock_(&mutex), owns_(lock_->try_lock_until(tp)) {}

  /// @copydoc
  /// roo_threads::doc::unique_lock::unique_lock(mutex_type&,roo_time::Duration)
  unique_lock(mutex_type& mutex, const roo_time::Duration& duration)
      : lock_(&mutex), owns_(lock_->try_lock_for(duration)) {}

  /// @copydoc roo_threads::doc::unique_lock::~unique_lock
  ~unique_lock() {
    if (owns_) unlock();
  }

  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;

  /// @copydoc roo_threads::doc::unique_lock::unique_lock(unique_lock&&)
  unique_lock(unique_lock&& lock) noexcept
      : lock_(lock.lock_), owns_(lock.owns_) {
    lock.lock_ = 0;
    lock.owns_ = false;
  }

  /// @copydoc roo_threads::doc::unique_lock::operator=(unique_lock&&)
  unique_lock& operator=(unique_lock&& lock) noexcept {
    if (owns_) unlock();

    unique_lock(roo::move(lock)).swap(*this);

    lock.lock_ = 0;
    lock.owns_ = false;

    return *this;
  }

  /// @copydoc roo_threads::doc::unique_lock::lock
  void lock() {
    lock_->lock();
    owns_ = true;
  }

  /// @copydoc roo_threads::doc::unique_lock::try_lock
  bool try_lock() {
    owns_ = lock_->try_lock();
    return owns_;
  }

  /// @copydoc roo_threads::doc::unique_lock::try_lock_until
  bool try_lock_until(const roo_time::Uptime& tp) {
    owns_ = lock_->try_lock_until(tp);
    return owns_;
  }

  /// @copydoc roo_threads::doc::unique_lock::try_lock_for
  bool try_lock_for(const roo_time::Duration& duration) {
    owns_ = lock_->try_lock_for(duration);
    return owns_;
  }

  /// @copydoc roo_threads::doc::unique_lock::unlock
  void unlock() {
    if (lock_ != nullptr) {
      lock_->unlock();
      owns_ = false;
    }
  }

  /// @copydoc roo_threads::doc::unique_lock::swap
  void swap(unique_lock& other) noexcept {
    std::swap(lock_, other.lock_);
    std::swap(owns_, other.owns_);
  }

  /// @copydoc roo_threads::doc::unique_lock::release
  mutex_type* release() noexcept {
    mutex_type* ret = lock_;
    lock_ = nullptr;
    owns_ = false;
    return ret;
  }

  /// @copydoc roo_threads::doc::unique_lock::owns_lock
  bool owns_lock() const noexcept { return owns_; }

  /// @copydoc roo_threads::doc::unique_lock::operator bool
  explicit operator bool() const noexcept { return owns_lock(); }

  /// @copydoc roo_threads::doc::unique_lock::mutex
  mutex_type* mutex() const noexcept { return lock_; }

 private:
  mutex_type* lock_;
  bool owns_;
};

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD