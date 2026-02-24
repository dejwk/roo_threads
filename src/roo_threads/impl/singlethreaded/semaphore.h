#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

#include "roo_backport.h"
#include "roo_backport/move.h"

namespace roo_threads {
/// @brief Backend namespace for single-threaded synchronization fallbacks.
namespace singlethreaded {

template <std::ptrdiff_t LeastMaxValue =
              std::numeric_limits<std::ptrdiff_t>::max()>
/// @ingroup roo_threads_api_semaphore
/// @brief Single-threaded backend implementation of `roo::counting_semaphore`.
/// @copydoc roo_threads::doc::counting_semaphore
class counting_semaphore {
 public:
  counting_semaphore() noexcept = default;
  counting_semaphore(const counting_semaphore&) = delete;
  counting_semaphore& operator=(const counting_semaphore&) = delete;

  /// @copydoc roo_threads::doc::counting_semaphore::acquire
  void acquire() noexcept {}
  /// @copydoc roo_threads::doc::counting_semaphore::release
  void release() noexcept {}
};

/// @ingroup roo_threads_api_semaphore
/// @brief Single-threaded backend implementation of `roo::binary_semaphore`.
/// @copydoc roo_threads::doc::binary_semaphore
using binary_semaphore = counting_semaphore<1>;

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD