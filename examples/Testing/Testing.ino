
#include "Arduino.h"
#include "roo_threads.h"
#include "roo_threads/thread.h"

void run(int tid) {
  // Threads are mapped onto FreeRTOS tasks. You can use FeeRTOS task APIs to
  // interact with them in a platform-specific way.
  TaskHandle_t handle = xTaskGetCurrentTaskHandle();
  Serial.print(pcTaskGetName(handle));
  Serial.print(" (0x");
  Serial.print((intptr_t)handle, HEX);
  Serial.print("): priority: ");
  Serial.print(uxTaskPriorityGet(handle));
  Serial.print(", free stack: ");
  Serial.println(uxTaskGetStackHighWaterMark(handle) * sizeof(StackType_t));
}

void setup() { Serial.begin(115200); }

void Start(const char* name) {
  Serial.print(name);
  Serial.print(": ");
  Serial.flush();
}

void Test(bool value) { Serial.println(value ? "Ok" : "FAILED"); }

struct CurrentTest {
  const char* name;
  bool passed;
};

CurrentTest current_test;

#define START(test_name)              \
  do {                                \
    current_test = {test_name, true}; \
    Serial.print(test_name);          \
    Serial.println("...");            \
    Serial.flush();                   \
  } while (0)

#define EXPECT(cond)                                      \
  do {                                                    \
    if (!(cond)) {                                        \
      Serial.print("  FAIL: " #cond " at " __FILE__ ":"); \
      Serial.print(__LINE__);                             \
      Serial.println();                                   \
      current_test.passed = false;                        \
    }                                                     \
  } while (0)

#define CONCLUDE()                                         \
  do {                                                     \
    Serial.print(current_test.name);                       \
    Serial.print(": ");                                    \
    Serial.println(current_test.passed ? "OK" : "FAILED"); \
    Serial.flush();                                        \
  } while (0)

void ThreadId() {
  START("ThreadId");
  roo::thread t1;
  EXPECT(t1.get_id() == roo::thread::id());
  roo::thread t2([]() {});
  roo::thread::id id2 = t2.get_id();
  EXPECT(id2 != roo::thread::id());
  EXPECT(id2 != t1.get_id());
  roo::thread t3([]() {});
  roo::thread::id id3 = t3.get_id();
  EXPECT(id2 != id3);
  t2.join();
  t3.join();
  CONCLUDE();
}

void ThreadMove() {
  START("ThreadMove");
  bool run = false;
  roo::thread t0([&]() { run = true; });
  roo::thread t1;
  t1 = std::move(t0);
  t1.join();
  EXPECT(run);
  CONCLUDE();
}

void ThreadJoinable() {
  START("ThreadJoinable");
  roo::thread t0;
  EXPECT(!t0.joinable());
  roo::thread t1 = std::move(t0);
  EXPECT(!t0.joinable());
  EXPECT(!t1.joinable());
  roo::thread t2([]() {});
  EXPECT(t2.joinable());
  roo::thread t3 = std::move(t2);
  EXPECT(t3.joinable());
  EXPECT(!t2.joinable());
  t3.join();
  CONCLUDE();
}

void ThreadSleep() {
  START("ThreadSleep");
  auto start = roo_time::Uptime::Now();
  roo::this_thread::sleep_for(roo_time::Millis(100));
  auto mid = roo_time::Uptime::Now();
  roo::this_thread::sleep_until(mid + roo_time::Millis(120));
  auto end = roo_time::Uptime::Now();
  auto delta1 = mid - start;
  auto delta2 = end - mid;
  EXPECT(delta1.inMillisRoundedUp() >= 100);
  EXPECT(delta1.inMillisRoundedUp() < 110);
  EXPECT(delta2.inMillisRoundedUp() >= 120);
  EXPECT(delta2.inMillisRoundedUp() < 130);
  CONCLUDE();
}

void ThisThreadId() {
  START("ThisThreadId");
  roo::thread::id main_id = roo::this_thread::get_id();
  EXPECT(main_id != roo::thread::id());
  roo::thread t1([&]() {
    roo::thread::id t1_id = roo::this_thread::get_id();
    EXPECT(t1_id != roo::thread::id());
    EXPECT(t1_id != main_id);
  });
  t1.join();
  CONCLUDE();
}

void RunThreadTests() {
  ThreadId();
  ThreadMove();
  ThreadJoinable();
  ThreadSleep();
  ThisThreadId();
}

void MutexLockUnlock() {
  START("MutexLockUnlock");
  roo::mutex mtx;
  mtx.lock();
  mtx.unlock();
  CONCLUDE();
}

void MutexLocks() {
  START("MutexLocks");
  bool flag = false;
  roo::mutex mtx;
  mtx.lock();
  roo::thread t1([&]() {
    EXPECT(!mtx.try_lock());
    mtx.lock();
    flag = true;
    mtx.unlock();
  });
  roo::this_thread::sleep_for(roo_time::Millis(100));
  EXPECT(!flag);
  mtx.unlock();
  t1.join();
  EXPECT(flag);
  CONCLUDE();
}

void LockGuardLocks() {
  START("LockGuardLocks");
  bool flag = false;
  roo::mutex mtx;
  roo::thread t1;
  {
    roo::lock_guard<roo::mutex> guard(mtx);
    t1 = roo::thread([&]() {
      EXPECT(!mtx.try_lock());
      mtx.lock();
      flag = true;
      mtx.unlock();
    });
    roo::this_thread::sleep_for(roo_time::Millis(100));
    EXPECT(!flag);
    // guard goes out of scope here
  }
  t1.join();
  EXPECT(flag);
  CONCLUDE();
}

void RunMutexTests() {
  MutexLockUnlock();
  MutexLocks();
  LockGuardLocks();
}

void CondWait() {
  START("CondWait");
  roo::mutex mtx;
  roo::condition_variable cond;
  bool flag = false;
  roo::thread t1([&]() {
    roo::unique_lock<roo::mutex> lock(mtx);
    while (!flag) {
      cond.wait(lock);
    }
  });
  roo::this_thread::sleep_for(roo_time::Millis(100));
  roo_time::Uptime flip = roo_time::Uptime::Now();
  {
    roo::lock_guard<roo::mutex> guard(mtx);
    flag = true;
    cond.notify_one();
  }
  t1.join();
  EXPECT((roo_time::Uptime::Now() - flip).inMillisRoundedUp() < 10);
  CONCLUDE();
}

void CondWaitForTimingOut() {
  START("CondWaitForTimingOut");
  roo::mutex mtx;
  roo::condition_variable cond;
  roo_time::Uptime start = roo_time::Uptime::Now();
  roo::thread t1([&]() {
    roo::unique_lock<roo::mutex> lock(mtx);
    auto result = cond.wait_for(lock, roo_time::Millis(100));
    EXPECT(result == roo::cv_status::timeout);
  });
  t1.join();
  EXPECT((roo_time::Uptime::Now() - start).inMillisRoundedUp() >= 100);
  EXPECT((roo_time::Uptime::Now() - start).inMillisRoundedUp() < 110);
  CONCLUDE();
}

void CondWaitForLongTimeout() {
  START("CondWaitForLongTimeout");
  roo::mutex mtx;
  roo::condition_variable cond;
  roo_time::Uptime start = roo_time::Uptime::Now();
  roo::thread t1([&]() {
    roo::unique_lock<roo::mutex> lock(mtx);
    auto result =
        cond.wait_for(lock, roo_time::Millis((((uint64_t)1) << 33) + 20));
    EXPECT(result == roo::cv_status::no_timeout);
  });
  roo::this_thread::sleep_for(roo_time::Millis(50));
  roo_time::Uptime flip = roo_time::Uptime::Now();
  cond.notify_one();
  t1.join();
  EXPECT((roo_time::Uptime::Now() - flip).inMillisRoundedUp() < 10);
  CONCLUDE();
}

void CondLostNotification() {
  START("CondLostNotification");
  roo::mutex mtx;
  roo::condition_variable cond;
  bool flag = false;
  bool waiting = false;
  roo_time::Uptime deadline = roo_time::Uptime::Now() + roo_time::Millis(1000);

  roo::thread t1([&]() {
    roo::unique_lock<roo::mutex> lock(mtx);
    waiting = true;
    while (!flag) {
      cond.wait_until(lock, deadline);
    }
  });

  while (!waiting) {
    roo::this_thread::sleep_for(roo_time::Millis(1));
    mtx.lock();
    // Memory barrier to ensure 'waiting' is read after acquiring the lock.
    mtx.unlock();
  }

  roo::unique_lock<roo::mutex> lock(mtx);
  roo::this_thread::sleep_for(roo_time::Millis(100));
  roo_time::Uptime flip_time = roo_time::Uptime::Now();
  flag = true;
  cond.notify_one();
  roo::this_thread::sleep_for(roo_time::Millis(20));
  lock.unlock();
  t1.join();
  roo_time::Duration delta = roo_time::Uptime::Now() - flip_time;
  EXPECT(delta.inMillisRoundedUp() < 30);  // Did not wait for full timeout.
  CONCLUDE();
}

void CondBroadcast() {
  START("CondBroadcast");
  roo::mutex mtx;
  roo::condition_variable cond;
  const int kNumThreads = 5;
  bool flag = false;
  roo::thread threads[kNumThreads];
  for (int i = 0; i < kNumThreads; ++i) {
    threads[i] = roo::thread([&]() {
      roo::unique_lock<roo::mutex> lock(mtx);
      while (flag == 0) {
        cond.wait(lock);
      }
    });
  }
  roo::this_thread::sleep_for(roo_time::Millis(10));
  roo_time::Uptime flip = roo_time::Uptime::Now();
  {
    roo::lock_guard<roo::mutex> guard(mtx);
    flag = true;
    cond.notify_all();
  }
  for (int i = 0; i < kNumThreads; ++i) {
    threads[i].join();
  }
  EXPECT((roo_time::Uptime::Now() - flip).inMillisRoundedUp() < 10);
  CONCLUDE();
}

void RunCondTests() {
  CondWait();
  CondWaitForTimingOut();
  CondWaitForLongTimeout();
  CondLostNotification();
  CondBroadcast();
}

void loop() {
  RunThreadTests();
  RunMutexTests();
  RunCondTests();
  delay(1000);
}
