#pragma once

#include "roo_threads/isr_support.h"

#if ROO_THREADS_HAS_ISR_SUPPORT

#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>

#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
#include <freertos/queue.h>

#include "roo_threads/impl/freertos/isr_yield.h"
#else
#include <cstring>

#include "roo_threads/impl/isr_critical_section.h"
#endif

namespace roo {

/// Copies interrupt requests into fixed-capacity FIFO storage. Construct
/// before enabling producers. Stop producers and the consumer before teardown.
template <typename T, std::size_t Capacity>
class isr_queue {
  static_assert(std::is_trivially_copyable<T>::value &&
                    !std::is_const<T>::value && !std::is_volatile<T>::value,
                "ISR records must be unqualified trivially copyable values");
  static_assert(Capacity > 0, "An ISR queue needs at least one slot");
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
  static_assert(Capacity <= std::numeric_limits<UBaseType_t>::max() / sizeof(T),
                "ISR queue size exceeds the FreeRTOS queue representation");
#endif
  static_assert(Capacity <= std::numeric_limits<std::size_t>::max() / sizeof(T),
                "ISR queue storage exceeds the addressable size");

 public:
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
  /// Creates an empty queue with Capacity slots prepared for ISR use.
  isr_queue() noexcept
      : queue_(xQueueCreateStatic(Capacity, sizeof(T), records_, &storage_)) {}

  /// Destroys the queue after its producers and consumer have stopped.
  ~isr_queue() { vQueueDelete(queue_); }
#else
  /// Creates an empty queue with Capacity slots prepared for ISR use.
  isr_queue() noexcept = default;
#endif

  isr_queue(const isr_queue&) = delete;
  isr_queue& operator=(const isr_queue&) = delete;

  /// Copies a record from an ISR and returns false if the queue is full.
  /// On FreeRTOS, the interrupt priority must permit kernel API calls.
  /// On ESP32, a cache-disabled handler requires the full call path and all
  /// accessed data, including this queue and the source, in internal memory.
  bool try_push_from_isr(const T& value) noexcept {
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
    BaseType_t task_woken = pdFALSE;
    const bool accepted =
        xQueueSendFromISR(queue_, std::addressof(value), &task_woken) == pdTRUE;
    roo_threads::internal::YieldFromIsr(task_woken);
    return accepted;
#else
    using Guard = roo_threads::internal::IsrLockGuard;
    Guard lock(lock_, Guard::Context::kIsr);
    if (count_ == Capacity) return false;
    const std::size_t to_end = Capacity - head_;
    const std::size_t tail = count_ < to_end ? head_ + count_ : count_ - to_end;
    std::memcpy(records_ + tail * sizeof(T), std::addressof(value), sizeof(T));
    ++count_;
    return true;
#endif
  }

  /// Copies and removes the oldest record in consumer context.
  /// Returns false and leaves value intact when empty.
  bool try_pop(T& value) noexcept {
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
    return xQueueReceive(queue_, std::addressof(value), 0) == pdTRUE;
#else
    roo_threads::internal::IsrLockGuard lock(lock_);
    if (count_ == 0) return false;
    std::memcpy(std::addressof(value), records_ + head_ * sizeof(T), sizeof(T));
    if (++head_ == Capacity) head_ = 0;
    --count_;
    return true;
#endif
  }

  /// Returns a snapshot of the record count in task or loop context.
  std::size_t size() const noexcept {
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
    return uxQueueMessagesWaiting(queue_);
#else
    roo_threads::internal::IsrLockGuard lock(lock_);
    return count_;
#endif
  }

 private:
  unsigned char records_[Capacity * sizeof(T)];
#ifdef ROO_THREADS_INTERNAL_ISR_FREERTOS
  StaticQueue_t storage_;
  const QueueHandle_t queue_;
#else
  mutable roo_threads::internal::IsrCriticalSection lock_;
  std::size_t head_ = 0;
  std::size_t count_ = 0;
#endif
};

}  // namespace roo

#endif
