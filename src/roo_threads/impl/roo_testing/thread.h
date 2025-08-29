#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_USE_ROO_TESTING

#define ROO_THREADS_ATTRIBUTES_SUPPORT_PRIORITY
#define ROO_THREADS_ATTRIBUTES_SUPPORT_NAME
#define ROO_THREADS_ATTRIBUTES_SUPPORT_STACK_SIZE

#include "roo_testing/sys/thread.h"

namespace roo_threads {
namespace roo_testing {

class thread : public ::roo_testing::thread {
 public:
  class attributes;

  explicit thread() : ::roo_testing::thread() {}

  template <typename Callable, typename... Args,
            typename std::enable_if<
                !std::is_same<Callable, thread::attributes>::value>::type>
  explicit thread(Callable&& callable, Args&&... args);

  template <typename Callable, typename... Args>
  explicit thread(const attributes& attrs, Callable&& callable, Args&&... args);
};

class thread::attributes {
 public:
  attributes() : stack_size_(0), priority_(-1), name_(nullptr) {}

  void set_stack_size(uint16_t stack_size) { stack_size_ = stack_size; }
  void set_priority(uint16_t priority) { priority_ = priority; }
  void set_name(const char* name) { name_ = name; }

 private:
  friend ::roo_testing::thread::attributes make_attrs(
      const thread::attributes& attrs);

  uint16_t stack_size_;
  int16_t priority_;
  const char* name_;
};

template <typename Callable, typename... Args,
          typename std::enable_if<
              !std::is_same<Callable, thread::attributes>::value>::type>
thread::thread(Callable&& callable, Args&&... args)
    : ::roo_testing::thread(callable, args...) {}

template <typename Callable, typename... Args>
thread::thread(const attributes& attrs, Callable&& callable, Args&&... args)
    : ::roo_testing::thread(make_attrs(attrs), callable, args...) {}

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