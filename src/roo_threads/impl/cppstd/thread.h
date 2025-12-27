#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#define ROO_THREADS_ATTRIBUTES_SUPPORT_PRIORITY 0
#define ROO_THREADS_ATTRIBUTES_SUPPORT_NAME 0
#define ROO_THREADS_ATTRIBUTES_SUPPORT_STACK_SIZE 0

#include <thread>

namespace roo_threads {
namespace cppstd {

using thread = std::thread;

namespace this_thread {

inline thread::id get_id() noexcept { return std::this_thread::get_id(); }

inline void yield() noexcept { std::this_thread::yield(); }

inline void sleep_for(const roo_time::Duration& duration) {
  static constexpr auto kMaxSafeWaitDelay =
      std::chrono::microseconds(10LL * 24 * 3600 * 1000000);
  auto delay = std::chrono::microseconds(duration.inMicros());
  if (delay > kMaxSafeWaitDelay) {
    // Protecting against overflow, e.g. wait_for(roo_time::Duration::Max()):
    // never wait for longer than 10 days; spuriously wake up if needed.
    // Using safely low max duration of 10 days, as ESP32 seems to overflow at
    // about 24 days.
    delay = kMaxSafeWaitDelay;
  }
  std::this_thread::sleep_for(delay);
}

inline void sleep_until(const roo_time::Uptime& when) {
  sleep_for(when - roo_time::Uptime::Now());
}

}  // namespace this_thread

}  // namespace cppstd
}  // namespace roo_threads

#endif