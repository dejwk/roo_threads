#include "roo_threads/condition_variable.h"

// #include <atomic>
// #include <chrono>
// #include <thread>

#include "gtest/gtest.h"
#include "roo_threads/condition_variable.h"
#include "roo_threads/mutex.h"
#include "roo_threads/thread.h"

TEST(ConditionVariableTest, NotifiesOneThread) {
  roo::mutex m;
  roo::condition_variable cv;
  bool ready = false;

  std::atomic<bool> thread_ran{false};
  roo::thread t([&] {
    roo::unique_lock<roo::mutex> lock(m);
    cv.wait(lock, [&] { return ready; });
    thread_ran = true;
  });

  roo::this_thread::sleep_for(roo_time::Millis(50));
  {
    roo::unique_lock<roo::mutex> lock(m);
    ready = true;
  }
  cv.notify_one();
  t.join();
  EXPECT_TRUE(thread_ran);
}

TEST(ConditionVariableTest, NotifiesAllThreads) {
  roo::mutex m;
  roo::condition_variable cv;
  bool ready = false;

  std::atomic<int> threads_ran{0};
  auto worker = [&] {
    roo::unique_lock<roo::mutex> lock(m);
    cv.wait(lock, [&] { return ready; });
    threads_ran++;
  };

  roo::thread t1(worker);
  roo::thread t2(worker);

  roo::this_thread::sleep_for(roo_time::Millis(50));
  {
    roo::unique_lock<roo::mutex> lock(m);
    ready = true;
  }
  cv.notify_all();
  t1.join();
  t2.join();
  EXPECT_EQ(threads_ran.load(), 2);
}

TEST(ConditionVariableTest, WaitForTimesOut) {
  roo::mutex m;
  roo::condition_variable cv;
  bool ready = false;

  std::atomic<bool> timed_out{false};
  roo::thread t([&] {
    roo::unique_lock<roo::mutex> lock(m);
    auto start = roo_time::Uptime::Now();
    timed_out =
        (cv.wait_for(lock, roo_time::Millis(1)) == roo::cv_status::timeout);
    auto end = roo_time::Uptime::Now();
    ASSERT_GE(end - start, roo_time::Millis(1));
  });

  t.join();
  EXPECT_TRUE(timed_out);
}

TEST(ConditionVariableTest, WaitForNotifiesBeforeTimeout) {
  roo::mutex m;
  roo::condition_variable cv;
  bool ready = false;

  std::atomic<bool> started{false};
  std::atomic<bool> notified{false};
  roo::thread t([&] {
    roo::unique_lock<roo::mutex> lock(m);
    started = true;
    notified =
        (cv.wait_for(lock, roo_time::Seconds(10)) == roo::cv_status::no_timeout);
  });

  // Let's give the worker some time to start and block on the CV.
  roo::this_thread::sleep_for(roo_time::Millis(50));
  cv.notify_one();
  t.join();
  EXPECT_TRUE(notified);
}
