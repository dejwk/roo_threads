#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_FREERTOS)

// If both cppstd and freertos are available (e.g. on ESP32), we prefer
// freertos, because it offers more control over thread attributes.

#include "roo_threads/impl/freertos/thread.h"

namespace roo {

using thread = ::roo_threads::freertos::thread;
namespace this_thread {

inline thread::id get_id() noexcept {
  return roo_threads::freertos::this_thread::get_id();
}

inline void yield() noexcept { roo_threads::freertos::this_thread::yield(); }

inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::freertos::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::freertos::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/thread.h"

namespace roo {

using thread = roo_threads::cppstd::thread;

namespace this_thread {

inline thread::id get_id() noexcept {
  return roo_threads::cppstd::this_thread::get_id();
}

inline void yield() noexcept { roo_threads::cppstd::this_thread::yield(); }

inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::cppstd::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::cppstd::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/thread.h"

namespace roo {

using thread = ::roo_threads::roo_testing::thread;

namespace this_thread {

inline thread::id get_id() noexcept {
  return roo_threads::roo_testing::this_thread::get_id();
}

inline void yield() noexcept { roo_threads::roo_testing::this_thread::yield(); }

inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::roo_testing::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::roo_testing::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/thread.h"

namespace roo {

using thread = roo_threads::singlethreaded::thread;

namespace this_thread {

inline thread::id get_id() noexcept {
  return roo_threads::singlethreaded::this_thread::get_id();
}

inline void yield() noexcept {
  roo_threads::singlethreaded::this_thread::yield();
}

inline void sleep_for(const roo_time::Duration& duration) {
  roo_threads::singlethreaded::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::singlethreaded::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#endif
