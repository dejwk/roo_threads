#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_ROO_TESTING

#include "roo_testing/sys/mutex.h"

// Locks of roo_testing are compliant with the C++ standard library lock
// templates.
#include "roo_threads/impl/cppstd/mutex.h"

namespace roo_threads {
namespace roo_testing {

using mutex = ::roo_testing::mutex;

template <typename Mutex>
using lock_guard = ::roo_testing::lock_guard<Mutex>;

using defer_lock_t = ::roo_testing::defer_lock_t;
using try_to_lock_t = ::roo_testing::try_to_lock_t;
using adopt_lock_t = ::roo_testing::adopt_lock_t;

// Tags for the unique_lock constructor.

inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};

template <typename Mutex>
using unique_lock = ::roo_testing::unique_lock<Mutex>;

}  // namespace roo_testing
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_ROO_TESTING
