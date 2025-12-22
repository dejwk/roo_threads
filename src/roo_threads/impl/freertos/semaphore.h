#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/timeutil.h"

namespace roo_threads {
namespace freertos {

template <std::ptrdiff_t LeastMaxValue =
              std::numeric_limits<UBaseType_t>::max()>
class counting_semaphore {
 public:
  explicit counting_semaphore(std::ptrdiff_t desired) noexcept {
    xSemaphoreCreateCountingStatic(LeastMaxValue, desired, &sem_);
  }

  counting_semaphore(const counting_semaphore&) = delete;

  counting_semaphore& operator=(const counting_semaphore&) = delete;

  void acquire() noexcept {
    xSemaphoreTake((SemaphoreHandle_t)&sem_, portMAX_DELAY);
  }

  bool try_acquire() noexcept {
    return xSemaphoreTake((SemaphoreHandle_t)&sem_, 0) == pdTRUE;
  }

  bool try_acquire_for(const roo_time::Duration& duration) noexcept {
    return try_acquire_until(internal::CalculateDeadlineFromDuration(duration));
  }

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

  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

class binary_semaphore {
 public:
  explicit binary_semaphore(std::ptrdiff_t desired) noexcept {
    xSemaphoreCreateBinaryStatic(&sem_);
    if (desired > 0) {
      xSemaphoreGive((SemaphoreHandle_t)&sem_);
    }
  }

  binary_semaphore(const binary_semaphore&) = delete;

  binary_semaphore& operator=(const binary_semaphore&) = delete;

  void acquire() noexcept {
    xSemaphoreTake((SemaphoreHandle_t)&sem_, portMAX_DELAY);
  }

  bool try_acquire() noexcept {
    return xSemaphoreTake((SemaphoreHandle_t)&sem_, 0) == pdTRUE;
  }

  bool try_acquire_for(const roo_time::Duration& duration) noexcept {
    return try_acquire_until(internal::CalculateDeadlineFromDuration(duration));
  }

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

  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS