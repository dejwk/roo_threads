#pragma once

#include "roo_threads/isr_support.h"

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
#include <freertos/task.h>

namespace roo_threads {
namespace internal {

// Use an object mux on ESP-IDF. Other supported ports provide critical
// sections through the kernel, including across cores on Arduino-Pico SMP.
class IsrCriticalSection {
 public:
#ifdef portMUX_INITIALIZER_UNLOCKED
  using State = bool;

  void lock() noexcept {
#ifdef portENTER_CRITICAL_IDF
    portENTER_CRITICAL_IDF(&mux_);
#else
    portENTER_CRITICAL(&mux_);
#endif
  }

  void unlock() noexcept {
#ifdef portEXIT_CRITICAL_IDF
    portEXIT_CRITICAL_IDF(&mux_);
#else
    portEXIT_CRITICAL(&mux_);
#endif
  }

  State lockFromIsr() noexcept {
    portENTER_CRITICAL_ISR(&mux_);
    return true;
  }

  void unlockFromIsr(State) noexcept { portEXIT_CRITICAL_ISR(&mux_); }

 private:
  portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
#else
  using State = decltype(taskENTER_CRITICAL_FROM_ISR());

  void lock() noexcept { taskENTER_CRITICAL(); }

  void unlock() noexcept { taskEXIT_CRITICAL(); }

  State lockFromIsr() noexcept { return taskENTER_CRITICAL_FROM_ISR(); }

  void unlockFromIsr(State state) noexcept {
    taskEXIT_CRITICAL_FROM_ISR(state);
  }
#endif
};

class IsrLockGuard {
 public:
  explicit IsrLockGuard(IsrCriticalSection& lock) noexcept
      : lock_(lock), state_(lock.lockFromIsr()) {}

  ~IsrLockGuard() { lock_.unlockFromIsr(state_); }

  IsrLockGuard(const IsrLockGuard&) = delete;
  IsrLockGuard& operator=(const IsrLockGuard&) = delete;

 private:
  IsrCriticalSection& lock_;
  IsrCriticalSection::State state_;
};

}  // namespace internal
}  // namespace roo_threads

#endif
