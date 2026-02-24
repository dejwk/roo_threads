#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

namespace roo_threads {

/// @brief Backend namespace for single-threaded fallback implementations.
namespace singlethreaded {

/// @ingroup roo_threads_api_thread
/// @brief Single-threaded backend implementation of `roo::thread`.
/// @copydoc roo_threads::doc::thread
class thread {
 public:
  /// @brief Single-threaded backend thread identifier.
  /// @copydoc roo_threads::doc::thread::id
  class id {
   public:
    /// @copydoc roo_threads::doc::thread::id::id
    id() : id_(0) {}
    id(int id) : id_(id) {}
    /// @copydoc roo_threads::doc::thread::id::operator==
    bool operator==(const id& other) const { return id_ == other.id_; }

   private:
    friend class thread;
    int id_;
  };

  /// @copydoc roo_threads::doc::thread::thread() noexcept
  thread() noexcept = default;

  thread(const thread&) = delete;

  /// @copydoc roo_threads::doc::thread::thread(thread&&)
  thread(thread&& other) noexcept { swap(other); }

  // Left intentionally unimplemented, as there is nothing we can do on a
  // single-threaded platform.
  /// @copydoc roo_threads::doc::thread::thread(Callable&&,Args&&...)
  /// @note Not available in single-threaded mode.
  template <typename Callable, typename... Args>
  explicit thread(Callable&& callable, Args&&... args);

  /// @copydoc roo_threads::doc::thread::~thread
  ~thread() = default;

  thread& operator=(const thread&) = delete;

  /// @copydoc roo_threads::doc::thread::operator=(thread&&)
  thread& operator=(thread&& other) noexcept = default;

  /// @copydoc roo_threads::doc::thread::swap
  void swap(thread& other) noexcept { std::swap(id_, other.id_); }

  /// @copydoc roo_threads::doc::thread::joinable
  bool joinable() const noexcept { return !(id_ == id()); }

  /// @copydoc roo_threads::doc::thread::join
  /// @note Not available in single-threaded mode.
  void join();  // Left unimplemented.

  /// @copydoc roo_threads::doc::thread::detach
  /// @note Not available in single-threaded mode.
  void detach();  // Left unimplemented.

  /// @copydoc roo_threads::doc::thread::get_id
  thread::id get_id() const noexcept { return id_; }

 private:
  id id_;
};

namespace this_thread {

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::get_id
thread::id get_id() noexcept { return thread::id(1); }

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::yield
/// @note Not available in single-threaded mode.
void yield() noexcept;  // Left unimplemented.

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::sleep_for
inline void sleep_for(const roo_time::Duration& duration) {
  if (duration.inMicros() <= 0) return;
  roo_time::Delay(duration);
}

/// @ingroup roo_threads_api_thread
/// @copydoc roo_threads::doc::this_thread::sleep_until
inline void sleep_until(const roo_time::Uptime& when) {
  return sleep_for(when - roo_time::Uptime::Now());
}

}  // namespace this_thread

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_SINGLETHREADED