#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_CPPSTD

#include <semaphore>

namespace roo_threads {
/// @brief Backend namespace using C++ standard library synchronization primitives.
namespace cppstd {

/// @ingroup roo_threads_api_semaphore
/// @brief C++ standard library backend implementation of
/// `roo::counting_semaphore`.
/// @copydoc roo_threads::doc::counting_semaphore
template <ptrdiff_t LeastMaxValue = std::counting_semaphore<>::max()>
using counting_semaphore = std::counting_semaphore<LeastMaxValue>;

/// @ingroup roo_threads_api_semaphore
/// @brief C++ standard library backend implementation of
/// `roo::binary_semaphore`.
/// @copydoc roo_threads::doc::binary_semaphore
using binary_semaphore = std::binary_semaphore;

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_CPPSTD