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

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/mutex.h"

namespace roo {

using mutex = ::roo_threads::roo_testing::mutex;

template <typename Mutex>
using lock_guard = ::roo_threads::roo_testing::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = ::roo_threads::roo_testing::unique_lock<Mutex>;

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
