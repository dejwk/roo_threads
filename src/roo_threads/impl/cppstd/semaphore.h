#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <semaphore>

namespace roo_threads {
namespace cppstd {

template <ptrdiff_t LeastMaxValue = std::counting_semaphore<>::max()>
using counting_semaphore = std::counting_semaphore<LeastMaxValue>;

using binary_semaphore = std::binary_semaphore;

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD