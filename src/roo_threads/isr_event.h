#pragma once

#include "roo_threads/isr_support.h"

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS

#include <freertos/semphr.h>

#include "roo_time.h"

namespace roo {

/// Stores a pending wakeup for one waiting task or loop. Construct before
/// enabling interrupt producers and stop producers and the waiter before
/// destroying the event.
class isr_event {
 public:
  /// Creates an event with no pending wakeup.
  isr_event() noexcept;

  /// Destroys the event after its producers and waiter have stopped.
  ~isr_event();

  isr_event(const isr_event&) = delete;
  isr_event& operator=(const isr_event&) = delete;

  /// Records a wakeup from task or loop context.
  void signal() noexcept;

  /// Records a wakeup from an ISR and requests a port yield when needed.
  /// On FreeRTOS, the interrupt priority must permit kernel API calls.
  /// On ESP32, a cache-disabled handler requires the full call path and all
  /// accessed data, including this event, in internal memory.
  void signal_from_isr() noexcept;

  /// Consumes a pending wakeup or waits once for an event. Recheck the work
  /// predicate after returning because a wait can return early.
  void wait() noexcept;

  /// Consumes a pending wakeup or waits once with an absolute deadline.
  /// An expired deadline returns immediately. Recheck work and time on return.
  void wait_until(roo_time::Uptime deadline) noexcept;

 private:
  StaticSemaphore_t storage_;
  const SemaphoreHandle_t semaphore_;
};

}  // namespace roo

#endif
