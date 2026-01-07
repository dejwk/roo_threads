#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "roo_threads/impl/freertos/config.h"
#include "roo_threads/impl/freertos/thread.h"

namespace roo_threads {
namespace freertos {

thread::attributes::attributes()
    : stack_size_(ROO_THREADS_FREERTOS_DEFAULT_THREAD_STACK_SIZE),
      priority_(ROO_THREADS_FREERTOS_DEFAULT_THREAD_PRIORITY),
      joinable_(true),
      name_("roo") {}

namespace {

struct thread_state {
  thread::attributes attr;

  std::unique_ptr<VirtualCallable> start = nullptr;
  TaskHandle_t task;
  StaticSemaphore_t join_barrier;
  StaticSemaphore_t join_mutex;
};

}  // namespace

void thread::swap(thread& other) noexcept { std::swap(state_, other.state_); }

bool thread::joinable() const noexcept {
  thread_state* state = (thread_state*)state_;
  return state != nullptr;
}

thread::~thread() { assert(!joinable()); }

thread& thread::operator=(thread&& other) noexcept {
  assert(!joinable());
  swap(other);
  return *this;
}

thread::id thread::get_id() const noexcept {
  thread_state* state = (thread_state*)state_;
  if (state_ == nullptr) {
    return thread::id(nullptr);
  }
  return thread::id(state->task);
}

static void run_thread(void* arg) {
  thread_state* p = (thread_state*)arg;
  std::unique_ptr<VirtualCallable> start = std::move(p->start);
  start->call();
  start = nullptr;
  if (p->attr.joinable()) {
    xSemaphoreGive((SemaphoreHandle_t)&p->join_barrier);
    vTaskSuspend(nullptr);
  } else {
    delete p;
    vTaskDelete(nullptr);
  }
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
  uint32_t stack_size_freertos =
      (uint32_t)(state->attr.stack_size() / sizeof(portSTACK_TYPE));
  // Note: we expect xTaskCreate to make a memory barrier, so that if the task
  // gets scheduled on a different core, it still sees fully initialized state.
  if (xTaskCreate(run_thread, state->attr.name(), stack_size_freertos,
                  (void*)state, state->attr.priority(),
                  &state->task) != pdPASS) {
    delete state;
    assert(false);
  }
  state_ = state;
}

void thread::join() {
  thread_state* state = (thread_state*)state_;
  assert(state != nullptr);        // Attempting to join a null thread
  assert(state->attr.joinable());  // Attempting to join a non-joinable thread
  if (xSemaphoreTake((SemaphoreHandle_t)&state->join_mutex, 0) != pdPASS) {
    assert(false);  // Another thread has already joined the requested thread
  }
  if (this_thread::get_id() == state->task) {
    assert(false);  // Thread attempting to join itself
  }

  // Wait for the joined thread to finish.
  xSemaphoreTake((SemaphoreHandle_t)&state->join_barrier, portMAX_DELAY);
  xSemaphoreGive((SemaphoreHandle_t)&state->join_barrier);
  // At this point, the joined thread does nothing else but suspends itself. It
  // is safe to delete resources.
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_barrier);
  state_ = 0;

  xSemaphoreGive((SemaphoreHandle_t)&state->join_mutex);
  vSemaphoreDelete((SemaphoreHandle_t)&state->join_mutex);
  vTaskDelete(state->task);

  // Nothing else should be dereferencing state anymoore.
  delete state;
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
