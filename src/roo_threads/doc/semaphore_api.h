#pragma once

#include <cstddef>

#include "roo_time.h"

namespace roo_threads {
namespace doc {

template <std::ptrdiff_t LeastMaxValue>
/// @ingroup roo_threads_api_semaphore
class counting_semaphore {
 public:
  /// @brief Constructs semaphore with initial token count.
  /// @param desired initial token count.
  explicit counting_semaphore(std::ptrdiff_t desired) noexcept;

  counting_semaphore(const counting_semaphore&) = delete;
  counting_semaphore& operator=(const counting_semaphore&) = delete;

  /// @brief Acquires a token, blocking until one is available.
  void acquire() noexcept;

  /// @brief Attempts to acquire a token without blocking.
  /// @return true if a token was acquired.
  bool try_acquire() noexcept;

  /// @brief Attempts to acquire a token for a bounded duration.
  /// @param duration relative timeout duration.
  /// @return true if a token was acquired.
  bool try_acquire_for(const roo_time::Duration& duration) noexcept;

  /// @brief Attempts to acquire a token until the given deadline.
  /// @param when absolute timeout point.
  /// @return true if a token was acquired.
  bool try_acquire_until(const roo_time::Uptime& when) noexcept;

  /// @brief Releases one token.
  void release() noexcept;
};

/// @ingroup roo_threads_api_semaphore
/// @brief Semaphore with maximum count of one.
using binary_semaphore = counting_semaphore<1>;

}  // namespace doc
}  // namespace roo_threads
