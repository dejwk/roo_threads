#pragma once

#include <cstdint>

#include "roo_time.h"

namespace roo_threads {
namespace doc {

/// @ingroup roo_threads_api_mutex
/// @brief Mutual exclusion primitive.
class mutex {
 public:
  /// @brief Constructs a mutex.
  mutex() noexcept;

  mutex(const mutex&) = delete;
  mutex& operator=(const mutex&) = delete;

  /// @brief Blocks until the mutex is acquired.
  void lock();

  /// @brief Attempts to acquire the mutex without blocking.
  /// @return true if lock acquisition succeeded.
  bool try_lock();

  /// @brief Releases the mutex.
  void unlock();
};

template <typename Mutex>
/// @ingroup roo_threads_api_mutex
class lock_guard {
 public:
  /// @brief Type of wrapped mutex.
  using mutex_type = Mutex;

  /// @brief Acquires mutex on construction.
  /// @param mutex mutex to lock.
  explicit lock_guard(mutex_type& mutex);

  /// @brief Releases mutex on destruction.
  ~lock_guard();
};

/// @ingroup roo_threads_api_mutex
/// @brief Tag type selecting deferred locking constructor overloads.
struct defer_lock_t;

/// @ingroup roo_threads_api_mutex
/// @brief Tag type selecting try-lock constructor overloads.
struct try_to_lock_t;

/// @ingroup roo_threads_api_mutex
/// @brief Tag type selecting adopt-lock constructor overloads.
struct adopt_lock_t;

template <typename Mutex>
/// @ingroup roo_threads_api_mutex
class unique_lock {
 public:
  /// @brief Type of wrapped mutex.
  using mutex_type = Mutex;

  /// @brief Constructs an empty lock not associated with any mutex.
  unique_lock() noexcept;

  /// @brief Acquires the given mutex.
  /// @param mutex mutex to lock.
  explicit unique_lock(mutex_type& mutex);

  /// @brief Associates with mutex but does not lock it.
  /// @param mutex mutex to associate with.
  unique_lock(mutex_type& mutex, defer_lock_t) noexcept;

  /// @brief Attempts to lock mutex without blocking.
  /// @param mutex mutex to lock.
  unique_lock(mutex_type& mutex, try_to_lock_t);

  /// @brief Adopts an already-owned mutex.
  /// @param mutex mutex already locked by caller.
  unique_lock(mutex_type& mutex, adopt_lock_t) noexcept;

  /// @brief Attempts to lock until the specified time point.
  /// @param mutex mutex to lock.
  /// @param tp absolute timeout point.
  unique_lock(mutex_type& mutex, roo_time::Uptime tp);

  /// @brief Attempts to lock for the specified duration.
  /// @param mutex mutex to lock.
  /// @param duration relative timeout duration.
  unique_lock(mutex_type& mutex, roo_time::Duration duration);

  /// @brief Releases owned mutex if any.
  ~unique_lock();

  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;

  /// @brief Move-constructs the lock.
  unique_lock(unique_lock&& lock) noexcept;

  /// @brief Move-assigns the lock.
  /// @return reference to this object.
  unique_lock& operator=(unique_lock&& lock) noexcept;

  /// @brief Acquires associated mutex.
  void lock();

  /// @brief Attempts to acquire associated mutex without blocking.
  /// @return true if lock acquired.
  bool try_lock();

  /// @brief Attempts to acquire associated mutex before the given deadline.
  /// @param tp absolute timeout point.
  /// @return true if lock acquired.
  bool try_lock_until(roo_time::Uptime tp);

  /// @brief Attempts to acquire associated mutex for the given duration.
  /// @param duration relative timeout duration.
  /// @return true if lock acquired.
  bool try_lock_for(roo_time::Duration duration);

  /// @brief Releases associated mutex if owned.
  void unlock();

  /// @brief Swaps two locks.
  void swap(unique_lock& other) noexcept;

  /// @brief Disassociates lock from mutex without unlocking.
  /// @return pointer to previously associated mutex.
  mutex_type* release() noexcept;

  /// @brief Returns whether this lock currently owns the mutex.
  bool owns_lock() const noexcept;

  /// @brief Boolean conversion equivalent to `owns_lock()`.
  explicit operator bool() const noexcept;

  /// @brief Returns associated mutex pointer, or `nullptr`.
  mutex_type* mutex() const noexcept;
};

}  // namespace doc
}  // namespace roo_threads
