#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_ATOMIC_USE_CPPSTD)

#include "roo_threads/impl/cppstd/atomic.h"

namespace roo {

using memory_order = roo_threads::cppstd::memory_order;

template <typename T>
using atomic = roo_threads::cppstd::atomic<T>;

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/atomic.h"

namespace roo {

using memory_order = roo_threads::singlethreaded::memory_order;

template <typename T>
using atomic = roo_threads::singlethreaded::atomic;

}  // namespace roo

#endif
