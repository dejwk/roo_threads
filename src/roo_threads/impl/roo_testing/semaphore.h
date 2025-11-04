#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_ROO_TESTING

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace roo_threads {
namespace roo_testing {

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

  void try_acquire() noexcept { xSemaphoreTake((SemaphoreHandle_t)&sem_, 0); }

  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

class binary_semaphore {
 public:
  explicit binary_semaphore(std::ptrdiff_t desired = 0) noexcept {
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

  void try_acquire() noexcept { xSemaphoreTake((SemaphoreHandle_t)&sem_, 0); }

  void release() noexcept { xSemaphoreGive((SemaphoreHandle_t)&sem_); }

 private:
  StaticSemaphore_t sem_;
};

}  // namespace roo_testing
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_ROO_TESTING
