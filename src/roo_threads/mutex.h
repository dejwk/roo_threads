#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/mutex.h"

namespace roo {

using mutex = ::roo_threads::cppstd::mutex;

template <typename Mutex>
using lock_guard = ::roo_threads::cppstd::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = ::roo_threads::cppstd::unique_lock<Mutex>;

using defer_lock_t = ::roo_threads::cppstd::defer_lock_t;
using try_to_lock_t = ::roo_threads::cppstd::try_to_lock_t;
using adopt_lock_t = ::roo_threads::cppstd::adopt_lock_t;

// Tags for the unique_lock constructor.

inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_USE_FREERTOS)

#include "roo_threads/impl/freertos/mutex.h"

namespace roo {

using mutex = ::roo_threads::freertos::mutex;
template <typename Mutex>
using lock_guard = ::roo_threads::freertos::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = ::roo_threads::freertos::unique_lock<Mutex>;

using defer_lock_t = ::roo_threads::freertos::defer_lock_t;
using try_to_lock_t = ::roo_threads::freertos::try_to_lock_t;
using adopt_lock_t = ::roo_threads::freertos::adopt_lock_t;
// Tags for the unique_lock constructor.

inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/mutex.h"

namespace roo {

using mutex = ::roo_threads::roo_testing::mutex;

template <typename Mutex>
using lock_guard = ::roo_threads::roo_testing::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = ::roo_threads::roo_testing::unique_lock<Mutex>;

using defer_lock_t = ::roo_threads::roo_testing::defer_lock_t;
using try_to_lock_t = ::roo_threads::roo_testing::try_to_lock_t;
using adopt_lock_t = ::roo_threads::roo_testing::adopt_lock_t;

// Tags for the unique_lock constructor.

inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/mutex.h"

namespace roo {

using mutex = ::roo_threads::singlethreaded::mutex;

template <typename Mutex>
using lock_guard = ::roo_threads::singlethreaded::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = ::roo_threads::singlethreaded::unique_lock<Mutex>;

}  // namespace roo

#endif
