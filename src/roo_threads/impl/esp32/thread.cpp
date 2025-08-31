#include "roo_threads/impl/esp32/thread.h"

#ifdef ROO_THREADS_USE_ESP32

#include "assert.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char* TAG = "roo_threads";

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
      joinable_(true),
      name_("roo_thread") {}

thread::thread() noexcept : state_(nullptr) {}

thread::thread(thread&& other) noexcept : state_(other.state_) {
  other.state_ = nullptr;
}

thread::~thread() { assert(!joinable()); }

thread& thread::operator=(thread&& other) noexcept {
  assert(!joinable());
  swap(other);
  return *this;
}

static void run_thread(void* arg) {
  thread_state* p = (thread_state*)arg;
  p->start->call();
  // Termination - block until we're joined.
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
  state_ = state;
  if (xTaskCreate(run_thread, state->attr.name(),
                  (uint16_t)(state->attr.stack_size() / sizeof(portSTACK_TYPE)),
                  (void*)state, state->attr.priority(),
                  &state->task) != pdPASS) {
    delete state;
    state_ = nullptr;
    ESP_LOGE(TAG, "Failed to create a new thread");
  }
  xTaskResumeAll();
}

void thread::join() {
  thread_state* state = state_;
  if (state == nullptr) {
    ESP_LOGE(TAG, "Thread joined for the second time");
    return;
  }
  assert(state->attr.joinable());
  if (this_thread::get_id() == get_id()) {
    ESP_LOGE(TAG, "Thread attempting to join itself");
    return;
  }
  if (xSemaphoreTake((SemaphoreHandle_t)&state->join_mutex, 0) != pdPASS) {
    ESP_LOGE(TAG, "Another thread has already joined the requested thread");
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
  TaskStatus_t task_status;
  vTaskGetInfo(nullptr, &task_status, pdFALSE, eRunning);
  return (intptr_t)task_status.xHandle;
}

void yield() noexcept { vPortYield(); }

void sleep_for(const roo_time::Interval& duration) {
  sleep_until(roo_time::Uptime::Now() + duration);
}

void sleep_until(const roo_time::Uptime& when) {
  while (true) {
    roo_time::Uptime now = roo_time::Uptime::Now();
    if (when <= now) return;
    roo_time::Interval delta = when - now;
    vTaskDelay((delta.inMillisRoundedUp() + portTICK_PERIOD_MS - 1) /
               portTICK_PERIOD_MS);
  }
}

}  // namespace this_thread

}  // namespace esp32
}  // namespace roo_threads

#endif