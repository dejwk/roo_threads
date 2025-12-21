#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

#include "roo_threads/impl/freertos/thread.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace roo_threads {
namespace freertos {

thread::attributes::attributes()
    : stack_size_(configMINIMAL_STACK_SIZE * sizeof(portSTACK_TYPE)),
      priority_(1),
      joinable_(true),
      name_("roo_testing") {}

namespace {

struct thread_state {
  thread::attributes attr;

  std::unique_ptr<internal::VirtualCallable> start = nullptr;
  TaskHandle_t task;
  StaticSemaphore_t join_barrier;
  StaticSemaphore_t join_mutex;
  void* result;
};

}  // namespace

thread::~thread() {
  assert(!joinable());
}

thread& thread::operator=(thread&& other) noexcept {
  assert(!joinable());
  swap(other);
  return *this;
}

thread::id thread::get_id() const noexcept {
  if (state_ == nullptr) {
    return thread::id(nullptr);
  }
  thread_state* state = (thread_state*)state_;
  return thread::id(state->task);
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

void thread::start(const attributes& attributes,
                   std::unique_ptr<internal::VirtualCallable> start) {
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
    assert(false);
  }
  state_ = state;
  xTaskResumeAll();
}

void thread::join() {
  thread_state* state = (thread_state*)state_;
  assert(state != nullptr);  // Attempting to join a null thread
  assert (state->attr.joinable());  // Attempting to join a non-joinable thread
  if (xSemaphoreTake((SemaphoreHandle_t)&state->join_mutex, 0) != pdPASS) {
    assert(false);  // Another thread has already joined the requested thread
  }
  if (this_thread::get_id() == state->task) {
    assert(false);  // Thread attempting to join itself
  }

  // Wait for the joined thread to finish.
  xSemaphoreTake((SemaphoreHandle_t)&state->join_barrier, portMAX_DELAY);

  vTaskSuspendAll();
  xSemaphoreGive((SemaphoreHandle_t)&state->join_barrier);
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_barrier);

  xSemaphoreGive((SemaphoreHandle_t)&state->join_mutex);
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_mutex);

  vTaskDelete(state->task);

  state_ = 0;
  delete state;
  xTaskResumeAll();
}

namespace this_thread {

thread::id get_id() noexcept { return thread::id(xTaskGetCurrentTaskHandle()); }

void yield() noexcept { vPortYield(); }

void sleep_for(const roo_time::Duration& duration) {
  sleep_until(roo_time::Uptime::Now() + duration);
}

void sleep_until(const roo_time::Uptime& when) {
  while (true) {
    roo_time::Uptime now = roo_time::Uptime::Now();
    if (when <= now) return;
    roo_time::Duration delta = when - now;
    vTaskDelay((delta.inMillisRoundedUp() + portTICK_PERIOD_MS - 1) /
               portTICK_PERIOD_MS);
  }
}

}  // namespace this_thread

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
