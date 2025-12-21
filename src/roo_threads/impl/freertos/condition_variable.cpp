#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

#include "roo_threads/impl/freertos/condition_variable.h"

namespace roo_threads {
namespace freertos {

#if defined(configNUM_CORES)
#if configNUM_CORES > 1
static portMUX_TYPE s_cond_lock = portMUX_INITIALIZER_UNLOCKED;
#define condENTER_CRITICAL() portENTER_CRITICAL(&s_cond_lock)
#define condEXIT_CRITICAL() portEXIT_CRITICAL(&s_cond_lock)
#else
#define condENTER_CRITICAL() vTaskEnterCritical()
#define condEXIT_CRITICAL() vTaskExitCritical()
#endif
#else
#define condENTER_CRITICAL() vTaskEnterCritical()
#define condEXIT_CRITICAL() vTaskExitCritical()
#endif

condition_variable::condition_variable() noexcept {
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    tasks_waiting_[i] = nullptr;
  }
}

void condition_variable::notify_one() noexcept {
  TaskHandle_t* task_to_notify = nullptr;
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] != nullptr) {
      if (task_to_notify == nullptr ||
          uxTaskPriorityGet(*task_to_notify) <
              uxTaskPriorityGet(tasks_waiting_[i])) {
        task_to_notify = &tasks_waiting_[i];
      }
    }
  }
  if (task_to_notify != nullptr) {
    xTaskNotify(*task_to_notify, 0, eNoAction);
    *task_to_notify = nullptr;
  }
  condEXIT_CRITICAL();
}

void condition_variable::notify_all() noexcept {
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] != nullptr) {
      xTaskNotify(tasks_waiting_[i], 0, eNoAction);
      tasks_waiting_[i] = nullptr;
    }
  }
  condEXIT_CRITICAL();
}

void condition_variable::wait(unique_lock<mutex>& lock) noexcept {
  TaskHandle_t me = xTaskGetCurrentTaskHandle();
  bool queued = false;
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] == nullptr) {
      tasks_waiting_[i] = me;
      queued = true;
      break;
    }
  }
  lock.unlock();
  condEXIT_CRITICAL();
  assert(queued);  // Checks for maximum number of queued threads reached.

  // Wait on the condition variable.
  // bool signaled =
  xTaskNotifyWait(0, 0, nullptr, portMAX_DELAY);  // == pdPASS;
  lock.lock();
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] == me) {
      tasks_waiting_[i] = nullptr;
      break;
    }
  }
  condEXIT_CRITICAL();
}

cv_status condition_variable::wait_until_impl(unique_lock<mutex>& lock,
                                              TickType_t when) noexcept {
  TickType_t now = xTaskGetTickCount();
  TickType_t delay = when - now;
  if (delay == 0 || delay > internal::kMaxTicksDelay) {
    return cv_status::timeout;
  }
  TaskHandle_t me = xTaskGetCurrentTaskHandle();
  bool queued = false;
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] == nullptr) {
      tasks_waiting_[i] = me;
      queued = true;
      break;
    }
  }
  lock.unlock();
  condEXIT_CRITICAL();
  cv_status status;
  assert(queued);  // Checks for maximum number of queued threads reached.
  // Wait on the condition variable.
  while (true) {
    bool signaled = (xTaskNotifyWait(0, 0, nullptr, delay) == pdPASS);
    if (signaled) {
      status = cv_status::no_timeout;
      break;
    }
    TickType_t now = xTaskGetTickCount();
    delay = when - now;
    if (delay == 0 || delay > internal::kMaxTicksDelay) break;
  }
  lock.lock();
  condENTER_CRITICAL();
  for (int i = 0; i < kMaxWaitingThreads; ++i) {
    if (tasks_waiting_[i] == me) {
      tasks_waiting_[i] = nullptr;
      break;
    }
  }
  condEXIT_CRITICAL();
  return status;
}

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS