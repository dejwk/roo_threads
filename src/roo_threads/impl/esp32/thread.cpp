#include "roo_threads/impl/esp32/thread.h"

#ifdef ROO_THREADS_USE_ESP32

#include "assert.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char *TAG = "roo_threads";

namespace roo_threads {
namespace esp32 {

struct thread_state {
  thread::attributes attr;

  std::unique_ptr<VirtualCallable> start = nullptr;
  TaskHandle_t task;
  StaticSemaphore_t join_barrier;
  StaticSemaphore_t join_mutex;
};

thread::attributes::attributes()
    : stack_size_(configMINIMAL_STACK_SIZE * sizeof(portSTACK_TYPE)),
      priority_(1),
      name_("roo_thread") {}

thread::~thread() { assert(!joinable()); }

thread& thread::operator=(thread&& other) noexcept {
  assert(!joinable());
  swap(other);
  return *this;
}

static void run_thread(void* arg) {
  thread_state* p = (thread_state*)arg;
  p->start->call();
  vTaskSuspendAll();
  if (p->attr.joinable()) {
    xSemaphoreGive((SemaphoreHandle_t)&p->join_barrier);
    xTaskResumeAll();
    vTaskSuspend(nullptr);
  } else {
    xTaskResumeAll();
    delete p;
    vTaskDelete(nullptr);
  }
}

thread::id thread::get_id() const noexcept {
  if (state_ == nullptr) return 0;
  return (intptr_t)state_->task;
}

void thread::start(const attributes& attributes,
                   std::unique_ptr<VirtualCallable> start) {
  thread_state* state = new thread_state;
  assert(state != nullptr);
  state->attr = attributes;
  state->start = std::move(start);
  if (state->attr.joinable()) {
    xSemaphoreCreateMutexStatic(&state->join_mutex);
    xSemaphoreCreateBinaryStatic(&state->join_barrier);
  }
  vTaskSuspendAll();
  if (xTaskCreate(run_thread, state->attr.name(),
                  (uint16_t)(state->attr.stack_size() / sizeof(portSTACK_TYPE)),
                  (void*)state, state->attr.priority(),
                  &state->task) != pdPASS) {
    delete state;
    ESP_LOGE(TAG, "Failed to create a new thread");
  }
  state_ = state;
  xTaskResumeAll();
}

void thread::join() {
  thread_state* state = state_;
  assert(state != nullptr);
  assert(state->attr.joinable());
  if (xSemaphoreTake((SemaphoreHandle_t)&state->join_mutex, 0) != pdPASS) {
    ESP_LOGE(TAG, "Another thread has already joined the requested thread");
  }
  if (this_thread::get_id() == get_id()) {
    ESP_LOGE(TAG, "Thread attempting to join itself");
  }

  // Wait for the joined thread to finish.
  xSemaphoreTake((SemaphoreHandle_t)&state->join_barrier, portMAX_DELAY);

  vTaskSuspendAll();
  xSemaphoreGive((SemaphoreHandle_t)&state->join_barrier);
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_barrier);

  xSemaphoreGive((SemaphoreHandle_t)&state->join_mutex);
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_mutex);

  vTaskDelete(state->task);

  state_ = nullptr;
  delete state;
  xTaskResumeAll();
}

namespace this_thread {

thread::id get_id() noexcept {
  TaskStatus_t* pxTaskStatus;
  vTaskGetInfo(nullptr, pxTaskStatus, pdFALSE, eRunning);
  return (intptr_t)pxTaskStatus->xHandle;
}

void yield() noexcept { vPortYield(); }

void sleep_for(const roo_time::Interval& duration) {
  uint64_t us = duration.inMicros();
  const TickType_t delay = (us + 999999) * configTICK_RATE_HZ / 1000000;
  if (delay > 0) {
    vTaskDelay(delay);
  }
}

void sleep_until(const roo_time::Uptime& when) {
  roo_time::Uptime now = roo_time::Uptime::Now();
  if (when > now) {
    sleep_for(when - now);
  }
}

}  // namespace this_thread

}  // namespace esp32
}  // namespace roo_threads

#endif