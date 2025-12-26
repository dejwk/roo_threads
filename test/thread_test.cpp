#include "roo_threads/thread.h"

#include "gtest/gtest.h"
#include "roo_threads/mutex.h"

TEST(Thread, SimpleStartAndJoin) {
  roo::thread t([] {});
  t.join();
}

TEST(Thread, StartAndJoinWithReturnValue) {
  std::atomic<int> value{0};
  roo::thread t([&] { value = 42; });
  t.join();
  EXPECT_EQ(value.load(), 42);
}

TEST(Thread, StartAndJoinMultipleTimes) {
  for (int i = 0; i < 5; ++i) {
    std::atomic<int> value{0};
    roo::thread t([&] { value = 42; });
    t.join();
    EXPECT_EQ(value.load(), 42);
  }
}

TEST(Thread, MoveConstructor) {
  std::atomic<int> value{0};
  roo::thread t1([&] { value = 42; });
  roo::thread t2(std::move(t1));
  t2.join();
  EXPECT_EQ(value.load(), 42);
}

TEST(Thread, MoveAssignment) {
  std::atomic<int> value{0};
  roo::thread t1([&] { value = 42; });
  roo::thread t2;
  t2 = std::move(t1);
  t2.join();
  EXPECT_EQ(value.load(), 42);
}

TEST(Thread, ThreadIdIsNonZeroAfterStart) {
  roo::thread t([] {});
  EXPECT_NE(t.get_id(), roo::thread::id());
  t.join();
}

TEST(Thread, ThreadIdIsZeroAfterJoin) {
  roo::thread t([] {});
  t.join();
  EXPECT_EQ(t.get_id(), roo::thread::id());
}

TEST(Thread, JoinableReturnsFalseAfterJoin) {
  roo::thread t([] {});
  t.join();
  EXPECT_FALSE(t.joinable());
}

TEST(Thread, ThreadRunsLambdaWithArguments) {
  std::atomic<int> value{0};
  int arg = 5;
  roo::thread t([&](int x) { value = x * 2; }, arg);
  t.join();
  EXPECT_EQ(value.load(), 10);
}

TEST(Thread, MultipleThreadsRunConcurrently) {
  std::atomic<int> counter{0};
  roo::thread t1([&] { counter++; });
  roo::thread t2([&] { counter++; });
  t1.join();
  t2.join();
  EXPECT_EQ(counter.load(), 2);
}

TEST(Thread, ThreadWithCustomAttributes) {
  roo::thread::attributes attr;
  attr.set_stack_size(2048);
  attr.set_priority(2);
  attr.set_name("custom_thread");
  std::atomic<bool> ran{false};
  roo::thread t(attr, [&] { ran = true; });
  t.join();
  EXPECT_TRUE(ran.load());
}

roo::thread::attributes CreateAttr() {
  roo::thread::attributes attr;
  attr.set_stack_size(2048);
  attr.set_priority(2);
  attr.set_name("custom_thread");
  return attr;
}

TEST(Thread, ThreadWithCustomAttributesByValue) {
  std::atomic<bool> ran{false};
  roo::thread t(CreateAttr(), [&] { ran = true; });
  t.join();
  EXPECT_TRUE(ran.load());
}

TEST(Thread, CannotJoinTwice) {
  roo::thread t([] {});
  t.join();
  EXPECT_DEATH(t.join(), "");
}

TEST(Thread, ThreadSelfJoinCrashes) {
  roo::thread t([&] { EXPECT_DEATH(t.join(), ""); });
  roo::this_thread::yield();
  t.join();
}

void func(int x, std::atomic<int>* value) { *value = x + 1; }

TEST(Thread, ThreadRunsFunctionPointerWithArgument) {
  std::atomic<int> value{0};
  roo::thread t(func, 41, &value);
  t.join();
  EXPECT_EQ(value.load(), 42);
}

TEST(Thread, ThreadRunsMemberFunctionWithArgument) {
  struct Foo {
    int val = 0;
    void set(int x) { val = x; }
  };
  Foo foo;
  roo::thread t(&Foo::set, &foo, 123);
  t.join();
  EXPECT_EQ(foo.val, 123);
}

TEST(Thread, ThreadRunsLambdaWithMultipleArguments) {
  std::atomic<int> value{0};
  roo::thread t([&](int x, int y) { value = x + y; }, 10, 32);
  t.join();
  EXPECT_EQ(value.load(), 42);
}

TEST(Thread, ThreadRunsStdFunctionWithArgument) {
  std::atomic<int> value{0};
  std::function<void(int)> f = [&](int x) { value = x * x; };
  roo::thread t(f, 6);
  t.join();
  EXPECT_EQ(value.load(), 36);
}

TEST(Thread, ThreadRunsWithReferenceArgument) {
  int x = 5;
  roo::thread t([&](int& y) { y = 99; }, std::ref(x));
  t.join();
  EXPECT_EQ(x, 99);
}

TEST(Thread, ThreadRunsWithConstReferenceArgument) {
  const int x = 7;
  std::atomic<int> value{0};
  roo::thread t([&](const int& y) { value = y * 2; }, std::cref(x));
  t.join();
  EXPECT_EQ(value.load(), 14);
}
TEST(ThisThread, GetIdReturnsNonZero) {
  roo::thread::id main_id = roo::this_thread::get_id();
  EXPECT_NE(main_id, roo::thread::id());
}

TEST(ThisThread, YieldDoesNotThrow) {
  EXPECT_NO_THROW(roo::this_thread::yield());
}

TEST(ThisThread, SleepForSleepsAtLeastRequestedTime) {
  using namespace roo_time;
  Uptime before = Uptime::Now();
  roo::this_thread::sleep_for(roo_time::Millis(50));
  Uptime after = Uptime::Now();
  EXPECT_GE((after - before).inMillis(), 50);
}

TEST(ThisThread, SleepUntilSleepsUntilRequestedTime) {
  using namespace roo_time;
  Uptime target = Uptime::Now() + roo_time::Millis(30);
  roo::this_thread::sleep_until(target);
  Uptime after = Uptime::Now();
  EXPECT_GE(after.inMillis(), target.inMillis());
}

TEST(ThisThread, SleepForZeroDoesNotThrow) {
  EXPECT_NO_THROW(roo::this_thread::sleep_for(roo_time::Millis(0)));
}

TEST(ThisThread, SleepUntilPastTimeReturnsImmediately) {
  using namespace roo_time;
  Uptime before = Uptime::Now();
  roo::this_thread::sleep_until(before - Millis(10));
  Uptime after = Uptime::Now();
  EXPECT_LT((after - before).inMillis(), 10);
}
