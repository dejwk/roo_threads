#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/mutex.h"

namespace roo_threads {
namespace freertos {

mutex::mutex() noexcept {
#if !ROO_THREADS_FREERTOS_LAZY_INITIALIZE
  xSemaphoreCreateMutexStatic(&mutex_);
#endif
}

#if ROO_THREADS_FREERTOS_LAZY_INITIALIZE

#if defined(ESP32) && defined(configNUM_CORES)
#if configNUM_CORES > 1
static portMUX_TYPE s_mutex_init_lock = portMUX_INITIALIZER_UNLOCKED;
#define mutexENTER_CRITICAL() portENTER_CRITICAL(&s_mutex_init_lock)
#define mutexEXIT_CRITICAL() portEXIT_CRITICAL(&s_mutex_init_lock)
#else
#define mutexENTER_CRITICAL() vPortEnterCritical()
#define mutexEXIT_CRITICAL() vPortExitCritical()
#endif
#else
#define mutexENTER_CRITICAL() vPortEnterCritical()
#define mutexEXIT_CRITICAL() vPortExitCritical()
#endif

void mutex::ensureInitialized() noexcept {
  if (!initialized_) {
    mutexENTER_CRITICAL();
    if (!initialized_) {
      xSemaphoreCreateMutexStatic(&mutex_);
      initialized_ = true;
    }
    mutexEXIT_CRITICAL();
  }
}
#else
void mutex::ensureInitialized() noexcept {}
#endif

void mutex::lock() {
  ensureInitialized();
  if (xTaskGetCurrentTaskHandle() == nullptr) {
    // Scheduler not yet running; no-op.
    return;
  }
  xSemaphoreTake((SemaphoreHandle_t)&mutex_, portMAX_DELAY);
}

bool mutex::try_lock() {
  ensureInitialized();
  if (xTaskGetCurrentTaskHandle() == nullptr) {
    // Scheduler not yet running; no-op.
    return true;
  }
  return xSemaphoreTake((SemaphoreHandle_t)&mutex_, 0);
}

void mutex::unlock() {
  ensureInitialized();
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
