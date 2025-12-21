#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/semaphore.h"

namespace roo {

template <std::ptrdiff_t LeastMaxValue>
using counting_semaphore =
    roo_threads::cppstd::counting_semaphore<LeastMaxValue>;

using binary_semaphore = roo_threads::cppstd::binary_semaphore;

}  // namespace roo

#elif (defined ROO_THREADS_USE_FREERTOS)

#include "roo_threads/impl/freertos/semaphore.h"

namespace roo {

template <std::ptrdiff_t LeastMaxValue>
using counting_semaphore =
    roo_threads::freertos::counting_semaphore<LeastMaxValue>;

using binary_semaphore = roo_threads::freertos::binary_semaphore;

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/semaphore.h"

namespace roo {

template <std::ptrdiff_t LeastMaxValue>
using counting_semaphore =
    roo_threads::roo_testing::counting_semaphore<LeastMaxValue>;

using binary_semaphore = roo_threads::roo_testing::binary_semaphore;

}  // namespace roo

#elif (defined ROO_THREADS_USE_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/semaphore.h"

namespace roo {

template <std::ptrdiff_t LeastMaxValue>
using counting_semaphore =
    roo_threads::singlethreaded::counting_semaphore<LeastMaxValue>;

using binary_semaphore = roo_threads::singlethreaded::binary_semaphore;

}  // namespace roo

#endif
