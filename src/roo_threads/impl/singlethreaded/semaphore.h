#pragma once

#include "roo_threads/impl/resolve.h"
#include "roo_time.h"

#ifdef ROO_THREADS_SINGLETHREADED

#include "roo_backport.h"
#include "roo_backport/move.h"

namespace roo_threads {
namespace singlethreaded {

template <std::ptrdiff_t LeastMaxValue =
              std::numeric_limits<std::ptrdiff_t>::max()>
class counting_semaphore {
 public:
  counting_semaphore() noexcept = default;
  counting_semaphore( const counting_semaphore& ) = delete;
  counting_semaphore& operator=(const counting_semaphore&) = delete;

  void acquire() noexcept {}
  void release() noexcept {}
};

using binary_semaphore = counting_semaphore<1>;

}  // namespace singlethreaded
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD