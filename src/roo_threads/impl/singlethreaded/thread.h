#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

namespace roo_threads {
namespace singlethreaded {

class thread {
 public:
  class id {
   public:
    id() : id_(0) {}
    id(int id) : id_(id) {}
    bool operator==(const id& other) const { return id_ == other.id_; }

   private:
    friend class thread;
    int id_;
  };

  thread() noexcept = default;

  thread(const thread&) = delete;

  thread(thread&& other) noexcept { swap(other); }

  // Left intentionally unimplemented, as there is nothing we can do on a
  // single-threaded platform.
  template <typename Callable, typename... Args>
  explicit thread(Callable&& callable, Args&&... args);

  ~thread() = default;

  thread& operator=(const thread&) = delete;

  thread& operator=(thread&& other) noexcept = default;

  void swap(thread& other) noexcept { std::swap(id_, other.id_); }

  bool joinable() const noexcept { return !(id_ == id()); }

  void join();  // Left unimplemented.

  void detach();  // Left unimplemented.

  thread::id get_id() const noexcept { return id_; }

 private:
  id id_;
};

namespace this_thread {

thread::id get_id() noexcept { return thread::id(1); }

void yield() noexcept;  // Left unimplemented.

inline void sleep_for(const roo_time::Interval& duration) {
  if (duration.inMicros() <= 0) return;
  roo_time::Delay(duration);
}

inline void sleep_until(const roo_time::Uptime& when) {
  return sleep_for(when - roo_time::Uptime::Now());
}

}  // namespace this_thread

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_SINGLETHREADED