#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_FREERTOS)

/// If both cppstd and FreeRTOS are available (e.g. ESP32), prefer FreeRTOS
/// because it offers more control over thread attributes.

#include "roo_threads/impl/freertos/thread.h"

namespace roo {

/// @brief Thread abstraction for the selected backend.
using thread = ::roo_threads::freertos::thread;
namespace this_thread {

/// @brief Returns the id of the calling thread.
inline thread::id get_id() noexcept {
  return roo_threads::freertos::this_thread::get_id();
}

/// @brief Hints the scheduler to switch to another thread.
inline void yield() noexcept { roo_threads::freertos::this_thread::yield(); }

/// @brief Suspends execution for at least the specified duration.
/// @param duration sleep interval.
inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::freertos::this_thread::sleep_for(duration);
}

/// @brief Suspends execution until the specified uptime point.
/// @param when wake-up time.
inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::freertos::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/thread.h"

namespace roo {

/// @brief Thread abstraction for the selected backend.
using thread = roo_threads::cppstd::thread;

namespace this_thread {

/// @brief Returns the id of the calling thread.
inline thread::id get_id() noexcept {
  return roo_threads::cppstd::this_thread::get_id();
}

/// @brief Hints the scheduler to switch to another thread.
inline void yield() noexcept { roo_threads::cppstd::this_thread::yield(); }

/// @brief Suspends execution for at least the specified duration.
/// @param duration sleep interval.
inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::cppstd::this_thread::sleep_for(duration);
}

/// @brief Suspends execution until the specified uptime point.
/// @param when wake-up time.
inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::cppstd::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/thread.h"

namespace roo {

/// @brief Thread abstraction for the selected backend.
using thread = ::roo_threads::roo_testing::thread;

namespace this_thread {

/// @brief Returns the id of the calling thread.
inline thread::id get_id() noexcept {
  return roo_threads::roo_testing::this_thread::get_id();
}

/// @brief Hints the scheduler to switch to another thread.
inline void yield() noexcept { roo_threads::roo_testing::this_thread::yield(); }

/// @brief Suspends execution for at least the specified duration.
/// @param duration sleep interval.
inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::roo_testing::this_thread::sleep_for(duration);
}

/// @brief Suspends execution until the specified uptime point.
/// @param when wake-up time.
inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::roo_testing::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/thread.h"

namespace roo {

/// @brief Thread abstraction for the selected backend.
using thread = roo_threads::singlethreaded::thread;

namespace this_thread {

/// @brief Returns the id of the calling thread.
inline thread::id get_id() noexcept {
  return roo_threads::singlethreaded::this_thread::get_id();
}

/// @brief Hints the scheduler to switch to another thread.
inline void yield() noexcept {
  roo_threads::singlethreaded::this_thread::yield();
}

/// @brief Suspends execution for at least the specified duration.
/// @param duration sleep interval.
inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::singlethreaded::this_thread::sleep_for(duration);
}

/// @brief Suspends execution until the specified uptime point.
/// @param when wake-up time.
inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::singlethreaded::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#endif
