#pragma once

#include "roo_threads/isr_support.h"

#if ROO_THREADS_HAS_ISR_SUPPORT

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
#include <freertos/task.h>
#else
#include <hardware/sync.h>
#endif

namespace roo_threads {
namespace internal {

class IsrCriticalSection {
 public:
#if defined(ROO_THREADS_INTERNAL_ISR_PICO)
  using State = uint32_t;

  State lock() noexcept {
#if defined(ROO_TESTING)
    return save_and_disable_interrupts();
#else
    return spin_lock_blocking(
        spin_lock_instance(PICO_SPINLOCK_ID_STRIPED_FIRST));
#endif
  }

  void unlock(State state) noexcept {
#if defined(ROO_TESTING)
    restore_interrupts(state);
#else
    spin_unlock(spin_lock_instance(PICO_SPINLOCK_ID_STRIPED_FIRST), state);
#endif
  }

  State lockFromIsr() noexcept { return lock(); }

  void unlockFromIsr(State state) noexcept { unlock(state); }
#elif defined(portMUX_INITIALIZER_UNLOCKED)
  using State = bool;

  State lock() noexcept {
#ifdef portENTER_CRITICAL_IDF
    portENTER_CRITICAL_IDF(&mux_);
#else
    portENTER_CRITICAL(&mux_);
#endif
    return true;
  }

  void unlock(State) noexcept {
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

  State lock() noexcept {
    taskENTER_CRITICAL();
    return State{};
  }

  void unlock(State) noexcept { taskEXIT_CRITICAL(); }

  State lockFromIsr() noexcept { return taskENTER_CRITICAL_FROM_ISR(); }

  void unlockFromIsr(State state) noexcept {
    taskEXIT_CRITICAL_FROM_ISR(state);
  }
#endif
};

// Carries the local IRQ state in the guard rather than the shared lock.
class IsrLockGuard {
 public:
  enum class Context { kTask, kIsr };

  explicit IsrLockGuard(IsrCriticalSection& lock,
                        Context context = Context::kTask) noexcept
      : lock_(lock),
        context_(context),
        state_(context == Context::kIsr ? lock.lockFromIsr() : lock.lock()) {}

  ~IsrLockGuard() {
    if (context_ == Context::kIsr) {
      lock_.unlockFromIsr(state_);
    } else {
      lock_.unlock(state_);
    }
  }

  IsrLockGuard(const IsrLockGuard&) = delete;
  IsrLockGuard& operator=(const IsrLockGuard&) = delete;

 private:
  IsrCriticalSection& lock_;
  Context context_;
  IsrCriticalSection::State state_;
};

}  // namespace internal
}  // namespace roo_threads

#endif
