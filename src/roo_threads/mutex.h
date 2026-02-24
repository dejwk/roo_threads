#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/mutex.h"

namespace roo {

/// @brief Mutual exclusion primitive.
using mutex = ::roo_threads::cppstd::mutex;

/// @brief RAII lock that locks on construction and unlocks on destruction.
/// @tparam Mutex mutex type.
template <typename Mutex>
using lock_guard = ::roo_threads::cppstd::lock_guard<Mutex>;

/// @brief Movable lock wrapper with deferred/try/adopt locking options.
/// @tparam Mutex mutex type.
template <typename Mutex>
using unique_lock = ::roo_threads::cppstd::unique_lock<Mutex>;

/// @brief Tag type used to defer locking in `unique_lock` constructors.
using defer_lock_t = ::roo_threads::cppstd::defer_lock_t;
/// @brief Tag type used to request try-locking in `unique_lock` constructors.
using try_to_lock_t = ::roo_threads::cppstd::try_to_lock_t;
/// @brief Tag type used to adopt an already-locked mutex in `unique_lock`.
using adopt_lock_t = ::roo_threads::cppstd::adopt_lock_t;

/// Tags for `unique_lock` constructor overload selection.

/// @brief Tag constant selecting deferred locking constructor overloads.
inline constexpr defer_lock_t defer_lock{};
/// @brief Tag constant selecting try-lock constructor overloads.
inline constexpr try_to_lock_t try_to_lock{};
/// @brief Tag constant selecting adopt-lock constructor overloads.
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_USE_FREERTOS)

#include "roo_threads/impl/freertos/mutex.h"

namespace roo {

/// @brief Mutual exclusion primitive.
using mutex = ::roo_threads::freertos::mutex;
/// @brief RAII lock that locks on construction and unlocks on destruction.
/// @tparam Mutex mutex type.
template <typename Mutex>
using lock_guard = ::roo_threads::freertos::lock_guard<Mutex>;

/// @brief Movable lock wrapper with deferred/try/adopt locking options.
/// @tparam Mutex mutex type.
template <typename Mutex>
using unique_lock = ::roo_threads::freertos::unique_lock<Mutex>;

/// @brief Tag type used to defer locking in `unique_lock` constructors.
using defer_lock_t = ::roo_threads::freertos::defer_lock_t;
/// @brief Tag type used to request try-locking in `unique_lock` constructors.
using try_to_lock_t = ::roo_threads::freertos::try_to_lock_t;
/// @brief Tag type used to adopt an already-locked mutex in `unique_lock`.
using adopt_lock_t = ::roo_threads::freertos::adopt_lock_t;
/// Tags for `unique_lock` constructor overload selection.

/// @brief Tag constant selecting deferred locking constructor overloads.
inline constexpr defer_lock_t defer_lock{};
/// @brief Tag constant selecting try-lock constructor overloads.
inline constexpr try_to_lock_t try_to_lock{};
/// @brief Tag constant selecting adopt-lock constructor overloads.
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/mutex.h"

namespace roo {

/// @brief Mutual exclusion primitive.
using mutex = ::roo_threads::roo_testing::mutex;

/// @brief RAII lock that locks on construction and unlocks on destruction.
/// @tparam Mutex mutex type.
template <typename Mutex>
using lock_guard = ::roo_threads::roo_testing::lock_guard<Mutex>;

/// @brief Movable lock wrapper with deferred/try/adopt locking options.
/// @tparam Mutex mutex type.
template <typename Mutex>
using unique_lock = ::roo_threads::roo_testing::unique_lock<Mutex>;

/// @brief Tag type used to defer locking in `unique_lock` constructors.
using defer_lock_t = ::roo_threads::roo_testing::defer_lock_t;
/// @brief Tag type used to request try-locking in `unique_lock` constructors.
using try_to_lock_t = ::roo_threads::roo_testing::try_to_lock_t;
/// @brief Tag type used to adopt an already-locked mutex in `unique_lock`.
using adopt_lock_t = ::roo_threads::roo_testing::adopt_lock_t;

/// Tags for `unique_lock` constructor overload selection.

/// @brief Tag constant selecting deferred locking constructor overloads.
inline constexpr defer_lock_t defer_lock{};
/// @brief Tag constant selecting try-lock constructor overloads.
inline constexpr try_to_lock_t try_to_lock{};
/// @brief Tag constant selecting adopt-lock constructor overloads.
inline constexpr adopt_lock_t adopt_lock{};

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/mutex.h"

namespace roo {

/// @brief Mutual exclusion primitive.
using mutex = ::roo_threads::singlethreaded::mutex;

/// @brief RAII lock that locks on construction and unlocks on destruction.
/// @tparam Mutex mutex type.
template <typename Mutex>
using lock_guard = ::roo_threads::singlethreaded::lock_guard<Mutex>;

/// @brief Movable lock wrapper with deferred/try/adopt locking options.
/// @tparam Mutex mutex type.
template <typename Mutex>
using unique_lock = ::roo_threads::singlethreaded::unique_lock<Mutex>;

}  // namespace roo

#endif
