#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#include "roo_threads/impl/freertos/callable.h"
#include "roo_time.h"

#define ROO_THREADS_ATTRIBUTES_SUPPORT_PRIORITY 1
#define ROO_THREADS_ATTRIBUTES_SUPPORT_NAME 1
#define ROO_THREADS_ATTRIBUTES_SUPPORT_STACK_SIZE 1

namespace roo_threads {

/// @brief Backend namespace using FreeRTOS threading primitives.
namespace freertos {

/// @ingroup roo_threads_api_thread
/// @brief FreeRTOS backend implementation of `roo::thread`.
/// @copydoc roo_threads::doc::thread
class thread {
 public:
  /// @copydoc roo_threads::doc::thread::id
  class id;

  /// @brief FreeRTOS-specific thread attributes.
  /// @copydoc roo_threads::doc::thread::attributes
  class attributes {
   public:
    /// @copydoc roo_threads::doc::thread::attributes::attributes
    attributes();

    /// @copydoc roo_threads::doc::thread::attributes::stack_size
    uint32_t stack_size() const { return stack_size_; }
    /// @copydoc roo_threads::doc::thread::attributes::priority
    uint16_t priority() const { return priority_; }
    /// @copydoc roo_threads::doc::thread::attributes::joinable
    bool joinable() const { return joinable_; }
    /// @copydoc roo_threads::doc::thread::attributes::name
    const char* name() const { return name_; }

    /// @copydoc roo_threads::doc::thread::attributes::set_stack_size
    void set_stack_size(uint32_t stack_size) { stack_size_ = stack_size; }

    /// @copydoc roo_threads::doc::thread::attributes::set_priority
    void set_priority(uint16_t priority) { priority_ = priority; }

    /// @copydoc roo_threads::doc::thread::attributes::set_name
    void set_name(const char* name) { name_ = name; }

   private:
    friend class thread;

    uint32_t stack_size_;
    uint16_t priority_;
    bool joinable_;
    const char* name_;
  };

  /// @copydoc roo_threads::doc::thread::thread() noexcept
  thread() noexcept : state_(nullptr) {}

  thread(const thread&) = delete;

  /// @copydoc roo_threads::doc::thread::thread(thread&&)
  thread(thread&& other) noexcept : thread() { swap(other); }

  /// @copydoc roo_threads::doc::thread::thread(Callable&&,Args&&...)
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

  /// @copydoc roo_threads::doc::thread::thread(const
  /// attributes&,Callable&&,Args&&...)
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

  /// @copydoc roo_threads::doc::thread::~thread
  ~thread();

  thread& operator=(const thread&) = delete;

  /// @copydoc roo_threads::doc::thread::operator=(thread&&)
  thread& operator=(thread&& other) noexcept;

  /// @copydoc roo_threads::doc::thread::swap
  void swap(thread& other) noexcept;

  /// @copydoc roo_threads::doc::thread::joinable
  bool joinable() const noexcept;

  /// @copydoc roo_threads::doc::thread::join
  void join();

  /// @copydoc roo_threads::doc::thread::detach
  void detach();

  /// @copydoc roo_threads::doc::thread::get_id
  thread::id get_id() const noexcept;

 private:
  void start(const attributes& attributes, std::unique_ptr<VirtualCallable>);

  void* state_;
};

namespace this_thread {

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::get_id
thread::id get_id() noexcept;

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::yield
void yield() noexcept;

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::sleep_for
/// @note FreeRTOS delay resolution is scheduler-tick based.
void sleep_for(const roo_time::Duration& duration);

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::sleep_until
/// @note FreeRTOS delay resolution is scheduler-tick based.
void sleep_until(const roo_time::Uptime& when);

}  // namespace this_thread

class thread::id {
 public:
  /// @copydoc roo_threads::doc::thread::id::id
  id() : id_(nullptr) {}
  /// @copydoc roo_threads::doc::thread::id::operator==
  bool operator==(const id& other) const { return id_ == other.id_; }
  /// @copydoc roo_threads::doc::thread::id::operator!=
  bool operator!=(const id& other) const { return id_ != other.id_; }
  /// @copydoc roo_threads::doc::thread::id::operator<
  bool operator<(const id& other) const { return id_ < other.id_; }
  /// @copydoc roo_threads::doc::thread::id::operator<=
  bool operator<=(const id& other) const { return id_ <= other.id_; }
  /// @copydoc roo_threads::doc::thread::id::operator>
  bool operator>(const id& other) const { return id_ > other.id_; }
  /// @copydoc roo_threads::doc::thread::id::operator>=
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
