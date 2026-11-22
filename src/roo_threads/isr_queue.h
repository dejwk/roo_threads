#pragma once

#include "roo_threads/isr_support.h"

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS

#include <freertos/queue.h>

#include <cstddef>
#include <limits>
#include <type_traits>

#include "roo_threads/impl/freertos/isr_yield.h"

namespace roo {

/// Copies interrupt requests into fixed-capacity FIFO storage. Construct
/// before enabling producers. Stop producers and the consumer before teardown.
template <typename T, std::size_t Capacity>
class isr_queue {
  static_assert(std::is_trivially_copyable<T>::value &&
                    !std::is_const<T>::value && !std::is_volatile<T>::value,
                "ISR records must be unqualified trivially copyable values");
  static_assert(Capacity > 0, "An ISR queue needs at least one slot");
  static_assert(Capacity <= std::numeric_limits<UBaseType_t>::max() / sizeof(T),
                "ISR queue size exceeds the FreeRTOS queue representation");

 public:
  /// Creates an empty queue with Capacity slots prepared for ISR use.
  isr_queue() noexcept
      : queue_(xQueueCreateStatic(Capacity, sizeof(T), records_, &storage_)) {}

  /// Destroys the queue after its producers and consumer have stopped.
  ~isr_queue() { vQueueDelete(queue_); }

  isr_queue(const isr_queue&) = delete;
  isr_queue& operator=(const isr_queue&) = delete;

  /// Copies a record from an ISR and returns false if the queue is full.
  /// On FreeRTOS, the interrupt priority must permit kernel API calls.
  /// On ESP32, a cache-disabled handler requires the full call path and all
  /// accessed data, including this queue and the source, in internal memory.
  bool try_push_from_isr(const T& value) noexcept {
    BaseType_t task_woken = pdFALSE;
    const bool accepted =
        xQueueSendFromISR(queue_, &value, &task_woken) == pdTRUE;
    roo_threads::internal::YieldFromIsr(task_woken);
    return accepted;
  }

  /// Copies and removes the oldest record in consumer context.
  /// Returns false and leaves value intact when empty.
  bool try_pop(T& value) noexcept {
    return xQueueReceive(queue_, &value, 0) == pdTRUE;
  }

  /// Returns a snapshot of the record count in task or loop context.
  std::size_t size() const noexcept { return uxQueueMessagesWaiting(queue_); }

 private:
  uint8_t records_[Capacity * sizeof(T)];
  StaticQueue_t storage_;
  const QueueHandle_t queue_;
};

}  // namespace roo

#endif
