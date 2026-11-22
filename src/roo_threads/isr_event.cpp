#include "roo_threads/isr_event.h"

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS

#include "roo_threads/impl/freertos/isr_yield.h"
#include "roo_threads/impl/freertos/timeutil.h"

namespace roo {

isr_event::isr_event() noexcept
    : semaphore_(xSemaphoreCreateBinaryStatic(&storage_)) {}

isr_event::~isr_event() { vSemaphoreDelete(semaphore_); }

void isr_event::signal() noexcept { xSemaphoreGive(semaphore_); }

void isr_event::signal_from_isr() noexcept {
  BaseType_t task_woken = pdFALSE;
  xSemaphoreGiveFromISR(semaphore_, &task_woken);
  roo_threads::internal::YieldFromIsr(task_woken);
}

void isr_event::wait() noexcept { xSemaphoreTake(semaphore_, portMAX_DELAY); }

void isr_event::wait_until(roo_time::Uptime deadline) noexcept {
  if (deadline == roo_time::Uptime::Max()) {
    wait();
    return;
  }
  const roo_time::Uptime now = roo_time::Uptime::Now();
  const TickType_t ticks =
      deadline <= now
          ? 0
          : roo_threads::freertos::internal::ToTicks(deadline - now);
  xSemaphoreTake(semaphore_, ticks);
}

}  // namespace roo

#endif

#ifdef ROO_THREADS_INTERNAL_ISR_PICO

#include <hardware/sync.h>
#include <pico/time.h>

namespace roo {

isr_event::isr_event() noexcept = default;

isr_event::~isr_event() = default;

void isr_event::signal() noexcept {
  {
    isr_lock_guard lock(lock_);
    pending_ = true;
  }
  __sev();
}

void isr_event::signal_from_isr() noexcept {
  // The Pico critical section saves local IRQ state in either context.
  signal();
}

bool isr_event::takeSignal() noexcept {
  isr_lock_guard lock(lock_);
  const bool pending = pending_;
  pending_ = false;
  return pending;
}

void isr_event::wait() noexcept {
  if (!takeSignal()) __wfe();
}

void isr_event::wait_until(roo_time::Uptime deadline) noexcept {
  if (deadline == roo_time::Uptime::Max()) {
    wait();
    return;
  }
  if (!takeSignal() && deadline > roo_time::Uptime::Now()) {
    best_effort_wfe_or_timeout(
        from_us_since_boot(static_cast<uint64_t>(deadline.inMicros())));
  }
}

}  // namespace roo

#endif
