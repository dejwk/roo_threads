#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_ESP32

#define ROO_THREADS_ATTRIBUTES_SUPPORT_PRIORITY
#define ROO_THREADS_ATTRIBUTES_SUPPORT_NAME
#define ROO_THREADS_ATTRIBUTES_SUPPORT_STACK_SIZE

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/impl/esp32/invoker.h"
#include "roo_time.h"

namespace roo_threads {
namespace esp32 {

struct thread_state;

class thread {
 public:
  using id = intptr_t;

  // Note: attributes are a non-portable extension.
  class attributes {
   public:
    attributes();

    uint16_t stack_size() const { return stack_size_; }
    uint16_t priority() const { return priority_; }
    bool joinable() const { return joinable_; }
    const char* name() const { return name_; }

    void set_stack_size(uint16_t stack_size) { stack_size_ = stack_size; }

    void set_priority(uint16_t priority) { priority_ = priority; }

    void set_name(const char* name) { name_ = name; }

   private:
    friend class thread;

    uint16_t stack_size_;
    uint16_t priority_;
    bool joinable_;
    const char* name_;
  };

  thread() noexcept;

  thread(const thread&) = delete;

  thread(thread&& other) noexcept;

  template <typename Callable, typename... Args,
            typename = typename std::enable_if<
                !std::is_same<Callable, thread::attributes&>::value>::type,
            typename = typename std::enable_if<
                !std::is_same<Callable, thread>::value>::type>
  explicit thread(Callable&& callable, Args&&... args) {
    static_assert(std::is_invocable<typename std::decay<Callable>::type,
                                    typename std::decay<Args>::type...>::value,
                  "roo::thread argument must be invocable");
    start(attributes(),
          MakeDynamicCallableWithArgs(std::forward<Callable>(callable),
                                      std::forward<Args>(args)...));
  }

  template <typename Callable, typename... Args,
            typename = typename std::enable_if<
                !std::is_same<Callable, thread>::value>::type>
  explicit thread(const attributes& attrs, Callable&& callable,
                  Args&&... args) {
    static_assert(std::is_invocable<typename std::decay<Callable>::type,
                                    typename std::decay<Args>::type...>::value,
                  "roo::thread argument must be invocable");

    start(attrs, MakeDynamicCallableWithArgs(std::forward<Callable>(callable),
                                             std::forward<Args>(args)...));
  }

  ~thread();

  thread& operator=(const thread&) = delete;

  thread& operator=(thread&& other) noexcept;

  void swap(thread& other) noexcept { std::swap(state_, other.state_); }

  bool joinable() const noexcept { return state_ != nullptr; }

  void join();

  void detach();

  thread::id get_id() const noexcept;

 private:
  void start(const attributes& attributes, std::unique_ptr<VirtualCallable>);

  thread_state* state_;
};

namespace this_thread {

thread::id get_id() noexcept;

void yield() noexcept;

void sleep_for(const roo_time::Interval& duration);

void sleep_until(const roo_time::Uptime& when);

}  // namespace this_thread

}  // namespace esp32
}  // namespace roo_threads

#endif