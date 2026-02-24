#pragma once

#include <cstdint>

#include "roo_time.h"

namespace roo_threads {
namespace doc {

/// @ingroup roo_threads_api_thread
/// @brief Canonical documentation-only thread API.
///
/// This type exists only to provide a single source of truth for API docs
/// across backend-specific implementations selected via compile-time macros.
class thread {
 public:
  /// @brief Opaque thread identifier.
  class id;

  /// @brief Optional thread attributes used by backend-specific constructors.
  class attributes {
   public:
    /// @brief Creates default thread attributes.
    attributes();

    /// @brief Returns requested stack size in bytes.
    uint32_t stack_size() const;

    /// @brief Returns requested scheduler priority.
    uint16_t priority() const;

    /// @brief Returns whether the created thread is joinable.
    bool joinable() const;

    /// @brief Returns thread name, or `nullptr` if not set.
    const char* name() const;

    /// @brief Sets requested stack size in bytes.
    /// @param stack_size stack size in bytes.
    void set_stack_size(uint32_t stack_size);

    /// @brief Sets requested scheduler priority.
    /// @param priority scheduler priority.
    void set_priority(uint16_t priority);

    /// @brief Sets thread name.
    /// @param name zero-terminated name string.
    void set_name(const char* name);
  };

  /// @brief Constructs a non-joinable empty thread handle.
  thread() noexcept;

  thread(const thread&) = delete;

  /// @brief Move-constructs a thread handle.
  /// @param other source thread handle.
  thread(thread&& other) noexcept;

  /// @brief Starts a new thread using default attributes.
  /// @tparam Callable callable type.
  /// @tparam Args argument types.
  /// @param callable callable object.
  /// @param args arguments passed to callable.
  template <typename Callable, typename... Args>
  explicit thread(Callable&& callable, Args&&... args);

  /// @brief Starts a new thread using explicit attributes.
  /// @tparam Callable callable type.
  /// @tparam Args argument types.
  /// @param attrs backend-specific thread attributes.
  /// @param callable callable object.
  /// @param args arguments passed to callable.
  template <typename Callable, typename... Args>
  explicit thread(const attributes& attrs, Callable&& callable, Args&&... args);

  /// @brief Destroys the thread handle.
  ///
  /// Backend-specific behavior applies if the thread is still joinable.
  ~thread();

  thread& operator=(const thread&) = delete;

  /// @brief Move-assigns a thread handle.
  /// @param other source thread handle.
  /// @return reference to this object.
  thread& operator=(thread&& other) noexcept;

  /// @brief Swaps two thread handles.
  /// @param other thread handle to swap with.
  void swap(thread& other) noexcept;

  /// @brief Returns true if this handle represents an active thread.
  bool joinable() const noexcept;

  /// @brief Waits for the represented thread to complete.
  void join();

  /// @brief Detaches the represented thread from this handle.
  void detach();

  /// @brief Returns identifier of the represented thread.
  id get_id() const noexcept;
};

namespace this_thread {

/// @ingroup roo_threads_api_thread
/// @brief Returns identifier of the current thread.
thread::id get_id() noexcept;

/// @ingroup roo_threads_api_thread
/// @brief Hints the scheduler to run another thread.
void yield() noexcept;

/// @ingroup roo_threads_api_thread
/// @brief Blocks the current thread for at least the given duration.
/// @param duration sleep interval.
void sleep_for(const roo_time::Duration& duration);

/// @ingroup roo_threads_api_thread
/// @brief Blocks the current thread until the specified time point.
/// @param when wake-up time.
void sleep_until(const roo_time::Uptime& when);

}  // namespace this_thread

class thread::id {
 public:
  /// @brief Constructs an invalid thread identifier.
  id();

  /// @brief Equality comparison.
  bool operator==(const id& other) const;

  /// @brief Inequality comparison.
  bool operator!=(const id& other) const;

  /// @brief Strict weak ordering comparison.
  bool operator<(const id& other) const;

  /// @brief Less-than-or-equal comparison.
  bool operator<=(const id& other) const;

  /// @brief Greater-than comparison.
  bool operator>(const id& other) const;

  /// @brief Greater-than-or-equal comparison.
  bool operator>=(const id& other) const;
};

}  // namespace doc
}  // namespace roo_threads
