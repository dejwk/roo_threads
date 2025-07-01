#pragma once

#if (defined ROO_TESTING)
#include "roo_testing/sys/mutex.h"

namespace roo {

using mutex = roo_testing::mutex;

template <typename Mutex>
using lock_guard = roo_testing::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = roo_testing::unique_lock<Mutex>;

}

#elif (defined ESP32)
#include <mutex>

namespace roo {

using mutex = std::mutex;

template <typename Mutex>
using lock_guard = std::lock_guard<Mutex>;

template <typename Mutex>
using unique_lock = std::unique_lock<Mutex>;

}

#endif
