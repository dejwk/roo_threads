#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_ATOMIC_USE_CPPSTD

#include <atomic>

namespace roo_threads {
namespace cppstd {

using memory_order = std::memory_order;

template <typename T>
using atomic = std::atomic<T>;

}  // namespace cppstd
}  // namespace roo_threads

#endif  // ROO_THREADS_ATOMIC_USE_CPPSTD
