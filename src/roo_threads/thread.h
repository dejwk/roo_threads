#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_ESP32)

#include "roo_threads/impl/esp32/thread.h"

namespace roo {

using thread = roo_threads::esp32::thread;

namespace this_thread {

inline thread::id get_id() noexcept {
  return roo_threads::esp32::this_thread::get_id();
}

inline void yield() noexcept { roo_threads::esp32::this_thread::yield(); }

inline void sleep_for(const roo_time::Interval& duration) {
  roo_threads::esp32::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::esp32::this_thread::sleep_until(when);
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

inline void sleep_for(const roo_time::Interval& duration) {
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

inline void sleep_for(const roo_time::Interval& duration) {
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

inline void sleep_for(const roo_time::Interval& duration) {
  roo_threads::singlethreaded::this_thread::sleep_for(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  roo_threads::singlethreaded::this_thread::sleep_until(when);
}

}  // namespace this_thread

}  // namespace roo

#endif
