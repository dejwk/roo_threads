#include "roo_threads/impl/freertos/condition_variable_classic.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

namespace roo_threads {
namespace freertos {

condition_variable_classic::condition_variable_classic() noexcept
    : waitlist_(0), waitcount_(0) {}

void condition_variable_classic::notify_one() noexcept {
  lock_guard<mutex> guard(mutex_);
  if (waitcount_ > 0) {
    waitlist_.release();
    --waitcount_;
  }
}

void condition_variable_classic::notify_all() noexcept {
  lock_guard<mutex> guard(mutex_);
  for (int i = 0; i < waitcount_; ++i) {
    waitlist_.release();
  }
  waitcount_ = 0;
}

void condition_variable_classic::wait(unique_lock<mutex>& lock) noexcept {
  {
    lock_guard<mutex> guard(mutex_);
    ++waitcount_;
    lock.unlock();
  }
  waitlist_.acquire();
  lock.lock();
}

cv_status condition_variable_classic::wait_until(unique_lock<mutex>& lock,
                                                 const roo_time::Uptime& when) {
  TickType_t deadline =
      xTaskGetTickCount() + internal::ToTicks(when - roo_time::Uptime::Now());
  {
    lock_guard<mutex> guard(mutex_);
    ++waitcount_;
    lock.unlock();
  }
  bool acquired = waitlist_.try_acquire_until(when);
  lock.lock();
  if (acquired) {
    return cv_status::no_timeout;
  }
  lock_guard<mutex> guard(mutex_);
  --waitcount_;
  return cv_status::timeout;
}

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS