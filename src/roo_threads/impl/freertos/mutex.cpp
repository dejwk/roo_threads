#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/mutex.h"

namespace roo_threads {
namespace freertos {

mutex::mutex() noexcept { xSemaphoreCreateMutexStatic(&mutex_); }

void mutex::lock() {
  if (xTaskGetCurrentTaskHandle() == nullptr) {
    // Scheduler not yet running; no-op.
    return;
  }
  xSemaphoreTake((SemaphoreHandle_t)&mutex_, portMAX_DELAY);
}

bool mutex::try_lock() {
  if (xTaskGetCurrentTaskHandle() == nullptr) {
    // Scheduler not yet running; no-op.
    return true;
  }
  return xSemaphoreTake((SemaphoreHandle_t)&mutex_, 0);
}

void mutex::unlock() {
  if (xTaskGetCurrentTaskHandle() == nullptr) {
    // Scheduler not yet running; no-op.
    return;
  }
  xSemaphoreGive((SemaphoreHandle_t)&mutex_);
}

namespace internal {

void checkLockUnowned(const void* lock, bool owns) {
  assert(lock != nullptr && !owns);
}

void checkLockOwned(bool owns) { assert(owns); }

}  // namespace internal

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
