#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <limits>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "roo_threads/impl/freertos/timeutil.h"

namespace roo_threads {
/// @brief Backend namespace using FreeRTOS synchronization primitives.
namespace freertos {

// Note: it doesn't seem to make sense to make a 'lazy-initialize' version
// of semaphores, because they take the desired initial count which is
// usually greater than zero.

template <std::ptrdiff_t LeastMaxValue =
              std::numeric_limits<UBaseType_t>::max()>
/// @ingroup roo_threads_api_semaphore
/// @brief FreeRTOS backend implementation of `roo::counting_semaphore`.
/// @copydoc roo_threads::doc::counting_semaphore
class counting_semaphore {
 public:
  /// @copydoc roo_threads::doc::counting_semaphore::counting_semaphore
  explicit counting_semaphore(std::ptrdiff_t desired) noexcept {
    xSemaphoreCreateCountingStatic(LeastMaxValue, desired, &sem_);
  }

  counting_semaphore(const counting_semaphore&) = delete;

  counting_semaphore& operator=(const counting_semaphore&) = delete;

  /// @copydoc roo_threads::doc::counting_semaphore::acquire
  void acquire() noexcept {
    xSemaphoreTake((SemaphoreHandle_t)&sem_, portMAX_DELAY);
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire
  bool try_acquire() noexcept {
    return xSemaphoreTake((SemaphoreHandle_t)&sem_, 0) == pdTRUE;
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire_for
  bool try_acquire_for(const roo_time::Duration& duration) noexcept {
    return try_acquire_until(internal::CalculateDeadlineFromDuration(duration));
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire_until
  bool try_acquire_until(const roo_time::Uptime& when) noexcept {
    while (true) {
      roo_time::Uptime now = roo_time::Uptime::Now();
      if (when <= now) return false;
      roo_time::Duration delta = when - now;
      if (xSemaphoreTake((SemaphoreHandle_t)&sem_, internal::ToTicks(delta)) ==
          pdTRUE) {
        return true;
      }
    }
  }

  /// @copydoc roo_threads::doc::counting_semaphore::release
  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

/// @ingroup roo_threads_api_semaphore
/// @brief FreeRTOS backend implementation of `roo::binary_semaphore`.
/// @copydoc roo_threads::doc::binary_semaphore
class binary_semaphore {
 public:
  /// @brief Constructs a binary semaphore.
  /// @param desired initial token count (`0` or `1`).
  explicit binary_semaphore(std::ptrdiff_t desired) noexcept {
    xSemaphoreCreateBinaryStatic(&sem_);
    if (desired > 0) {
      xSemaphoreGive((SemaphoreHandle_t)&sem_);
    }
  }

  binary_semaphore(const binary_semaphore&) = delete;

  binary_semaphore& operator=(const binary_semaphore&) = delete;

  /// @copydoc roo_threads::doc::counting_semaphore::acquire
  void acquire() noexcept {
    xSemaphoreTake((SemaphoreHandle_t)&sem_, portMAX_DELAY);
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire
  bool try_acquire() noexcept {
    return xSemaphoreTake((SemaphoreHandle_t)&sem_, 0) == pdTRUE;
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire_for
  bool try_acquire_for(const roo_time::Duration& duration) noexcept {
    return try_acquire_until(internal::CalculateDeadlineFromDuration(duration));
  }

  /// @copydoc roo_threads::doc::counting_semaphore::try_acquire_until
  bool try_acquire_until(const roo_time::Uptime& when) noexcept {
    while (true) {
      roo_time::Uptime now = roo_time::Uptime::Now();
      if (when <= now) return false;
      roo_time::Duration delta = when - now;
      if (xSemaphoreTake((SemaphoreHandle_t)&sem_, internal::ToTicks(delta)) ==
          pdTRUE) {
        return true;
      }
    }
  }

  /// @copydoc roo_threads::doc::counting_semaphore::release
  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS