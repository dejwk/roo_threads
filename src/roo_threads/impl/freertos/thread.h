#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "roo_threads/impl/freertos/callable.h"
#include "roo_time.h"

#define ROO_THREADS_ATTRIBUTES_SUPPORT_PRIORITY
#define ROO_THREADS_ATTRIBUTES_SUPPORT_NAME
#define ROO_THREADS_ATTRIBUTES_SUPPORT_STACK_SIZE

namespace roo_threads {
namespace freertos {

class thread {
 public:
  // Turns a pointer into an opaque type.
  class id;

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

  thread() noexcept : state_(nullptr) {}

  thread(const thread&) = delete;

  thread(thread&& other) noexcept : thread() { swap(other); }

  template <
      typename Callable, typename... Args,
      typename = typename std::enable_if<!std::is_same<
          std::remove_reference_t<Callable>, thread::attributes>::value>::type,
      typename =
          typename std::enable_if<!std::is_same<Callable, thread>::value>::type>
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

  void swap(thread& other) noexcept;

  bool joinable() const noexcept;

  void join();

  void detach();

  thread::id get_id() const noexcept;

 private:
  void start(const attributes& attributes, std::unique_ptr<VirtualCallable>);

  void* state_;
};

namespace this_thread {

thread::id get_id() noexcept;

void yield() noexcept;

void sleep_for(const roo_time::Duration& duration);

void sleep_until(const roo_time::Uptime& when);

}  // namespace this_thread

class thread::id {
 public:
  id() : id_(nullptr) {}
  bool operator==(const id& other) const { return id_ == other.id_; }
  bool operator!=(const id& other) const { return id_ != other.id_; }
  bool operator<(const id& other) const { return id_ < other.id_; }
  bool operator<=(const id& other) const { return id_ <= other.id_; }
  bool operator>(const id& other) const { return id_ > other.id_; }
  bool operator>=(const id& other) const { return id_ >= other.id_; }

 private:
  id(void* id) : id_(id) {}

  friend class thread;
  friend id this_thread::get_id() noexcept;

  void* id_;
};

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
