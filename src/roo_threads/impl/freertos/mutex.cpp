#include "roo_threads/impl/freertos/mutex.h"

#include "freertos/semphr.h"
#include "freertos/task.h"

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
//   CHECK_NOTNULL(lock);
//   CHECK(!owns) << "Deadlock would occur";
}

void checkLockOwned(bool owns) {
//   CHECK(owns) << "Lock not owned";
}

}  // namespace internal

}  // namespace freertos
}  // namespace roo_threads
