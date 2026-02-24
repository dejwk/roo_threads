#pragma once

#include "roo_threads/condition_variable.h"
#include "roo_threads/mutex.h"

namespace roo {

/// @brief A synchronization aid that blocks until a counter reaches zero.
class latch {
 public:
  /// @brief Creates a latch with an initial counter value.
  /// @param count initial counter value.
  explicit latch(std::ptrdiff_t count) : count_(count) {}

  /// @brief Decrements the counter and wakes waiters when it reaches zero.
  /// @param n decrement amount.
  void count_down(std::ptrdiff_t n = 1) {
    roo::lock_guard<roo::mutex> lock(mutex_);
    if ((count_ -= n) == 0) {
      cv_.notify_all();  // Wake ALL waiters
    }
  }

  /// @brief Blocks until the internal counter reaches zero.
  void wait() const {
    roo::unique_lock<roo::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return count_ == 0; });
  }

 private:
  mutable roo::mutex mutex_;
  mutable roo::condition_variable cv_;
  std::ptrdiff_t count_;
};

}  // namespace roo
