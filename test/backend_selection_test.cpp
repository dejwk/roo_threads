#include <type_traits>

#include "gtest/gtest.h"
#include "roo_threads.h"
#include "roo_threads/semaphore.h"

#if defined(ROO_TESTING_RUNTIME_BARE_METAL) && ROO_TESTING_RUNTIME_BARE_METAL
#ifndef ROO_THREADS_SINGLETHREADED
#error Bare-metal emulation requires the single-threaded backend
#endif
#if defined(ROO_THREADS_USE_FREERTOS) || defined(ROO_THREADS_USE_ROO_TESTING)
#error Bare-metal emulation selected a threaded backend
#endif
static_assert(
    std::is_same<roo::mutex, roo_threads::singlethreaded::mutex>::value);
#else
#ifndef ROO_THREADS_USE_FREERTOS
#error Existing emulation profiles must retain FreeRTOS
#endif
#endif

// Verifies the selected public mutex, lock and clock APIs work without spawning
// threads; this is the supported bare-metal client contract.
TEST(BackendSelection, LockAndSleep) {
  roo::mutex mutex;
  roo::unique_lock<roo::mutex> lock(mutex);
  EXPECT_TRUE(lock.owns_lock());
  lock.unlock();
  const roo_time::Uptime before = roo_time::Uptime::Now();
  roo::this_thread::sleep_for(roo_time::Millis(2));
  EXPECT_GE(roo_time::Uptime::Now() - before, roo_time::Millis(2));
}
