#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <thread>

namespace roo_threads {
namespace cppstd {

using thread = std::thread;

namespace this_thread {

inline thread::id get_id() noexcept { return std::this_thread::get_id(); }

inline void yield() noexcept { std::this_thread::yield(); }

inline void sleep_for(const roo_time::Interval& duration) {
  std::this_thread::sleep_for(std::chrono::microseconds(duration.inMicros()));
}

inline void sleep_until(const roo_time::Uptime& when) {
  sleep_for(when - roo_time::Uptime::Now());
}

}  // namespace this_thread

}  // namespace cppstd
}  // namespace roo_threads

#endif