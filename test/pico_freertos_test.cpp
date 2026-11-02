#include <chrono>

#include "gtest/gtest.h"
#include "roo_testing/system/timer.h"
#include "roo_threads.h"
#include "roo_threads/semaphore.h"
#include "roo_time.h"

#if !defined(ROO_THREADS_USE_FREERTOS) || !defined(__FREERTOS)
#error Pico must use the real FreeRTOS backend
#endif
#if defined(ESP32) || defined(ESP_PLATFORM)
#error Foreign kernel identity
#endif

// Verifies roo_time delays release the kernel task and concurrent waits share
// one uptime domain rather than adding their durations to the manual clock.
TEST(PicoFreeRtos, SharedClockAndJoins) {
  const int64_t system_before = system_time_get_micros();
  const roo_time::Uptime before = roo_time::Uptime::Now();
  EXPECT_GE(before.inMicros(), system_before);
  EXPECT_LE(before.inMicros(), system_time_get_micros());
  roo::binary_semaphore done(0);
  roo::thread first([&done] {
    roo_time::Delay(roo_time::Millis(10));
    done.release();
  });
  roo::thread second([&done] {
    roo_time::Delay(roo_time::Millis(20));
    done.release();
  });
  done.acquire();
  done.acquire();
  first.join();
  second.join();
  const roo_time::Duration elapsed = roo_time::Uptime::Now() - before;
  EXPECT_GE(elapsed, roo_time::Millis(20));
  if (!system_time_is_auto_sync_enabled()) {
    EXPECT_EQ(elapsed, roo_time::Millis(20));
  }
}

// Verifies contended mutexes, CV notification, and timed waits use the selected
// kernel. A child must acquire the lock before it can satisfy the predicate.
TEST(PicoFreeRtos, ConditionVariableAndTimeout) {
  roo::mutex mutex;
  roo::condition_variable cv;
  bool ready = false;
  roo::unique_lock<roo::mutex> lock(mutex);
  roo::thread worker([&] {
    roo::unique_lock<roo::mutex> child_lock(mutex);
    ready = true;
    cv.notify_one();
  });
  EXPECT_TRUE(
      cv.wait_for(lock, roo_time::Millis(100), [&ready] { return ready; }));
  const roo_time::Uptime before = roo_time::Uptime::Now();
  EXPECT_EQ(cv.wait_for(lock, roo_time::Millis(5)), roo::cv_status::timeout);
  EXPECT_GE(roo_time::Uptime::Now() - before, roo_time::Millis(4));
  lock.unlock();
  worker.join();
}
