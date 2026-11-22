#pragma once

#include "roo_threads/isr_support.h"

#if ROO_THREADS_HAS_ISR_SUPPORT

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
#include <freertos/task.h>
#else
#include <hardware/sync.h>
#endif

namespace roo {

/// Protects shared data accessed from task, loop, and interrupt contexts.
/// Construct before enabling interrupts that use it. Stop its users before
/// destroying it. Acquire it with isr_lock_guard for the calling context.
class isr_critical_section {
 public:
  /// Creates a critical section that is ready to use.
  isr_critical_section() noexcept = default;

  isr_critical_section(const isr_critical_section&) = delete;
  isr_critical_section& operator=(const isr_critical_section&) = delete;

 private:
  friend class isr_lock_guard;

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

/// Holds an ISR-safe critical section for the lifetime of the guard.
/// Keep the scope short. Release the guard before waiting, signaling a wake
/// event, or acquiring another ISR guard. Some ports share one underlying
/// lock across critical-section objects.
class isr_lock_guard {
 public:
  /// Identifies the context entering the critical section.
  enum class Context {
    kTask,  ///< Ordinary task or application loop.
    kIsr,   ///< Interrupt handler.
  };

  /// Acquires @p lock in @p context. On FreeRTOS, an ISR must run at a
  /// priority that permits kernel API calls. On ESP32 with cache disabled,
  /// the entire call path and all accessed data must be in internal memory.
  explicit isr_lock_guard(isr_critical_section& lock,
                          Context context = Context::kTask) noexcept
      : lock_(lock),
        context_(context),
        state_(context == Context::kIsr ? lock.lockFromIsr() : lock.lock()) {}

  /// Releases the critical section and restores the caller's interrupt state.
  ~isr_lock_guard() {
    if (context_ == Context::kIsr) {
      lock_.unlockFromIsr(state_);
    } else {
      lock_.unlock(state_);
    }
  }

  isr_lock_guard(const isr_lock_guard&) = delete;
  isr_lock_guard& operator=(const isr_lock_guard&) = delete;

 private:
  isr_critical_section& lock_;
  Context context_;
  isr_critical_section::State state_;
};

}  // namespace roo

#endif
