#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_ROO_TESTING

#include "roo_testing/sys/thread.h"

namespace roo_threads {
namespace roo_testing {

using thread = ::roo_testing::thread;

namespace this_thread {

inline thread::id get_id() noexcept {
  return ::roo_testing::this_thread::get_id();
}

inline void yield() noexcept { ::roo_testing::this_thread::yield(); }

inline void sleep_for(const roo_time::Interval& duration) {
  ::roo_testing::this_thread::sleep_for(
      std::chrono::microseconds(duration.inMicros()));
}

inline void sleep_until(const roo_time::Uptime& when) {
  sleep_for(when - roo_time::Uptime::Now());
}

}  // namespace this_thread

}  // namespace roo_testing
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_ROO_TESTING