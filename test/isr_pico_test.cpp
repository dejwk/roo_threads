#include <hardware/sync.h>
#include <pico/time.h>

#include <cstdint>

#include "gtest/gtest.h"
#include "roo_threads.h"

namespace {

struct Posts {
  roo::isr_queue<uint32_t, 3> queue;
  roo::isr_event event;
  uint32_t next = 1;
  unsigned rejected = 0;
  bool in_irq = false;
};

int64_t PostRecords(alarm_id_t, void* argument) {
  auto& posts = *static_cast<Posts*>(argument);
  posts.in_irq = __get_current_exception() != 0;
  for (unsigned i = 0; i < 4; ++i) {
    if (!posts.queue.try_push_from_isr(posts.next++)) ++posts.rejected;
  }
  posts.event.signal_from_isr();
  return 0;
}

void WaitForRecords(Posts& posts) {
  const auto deadline = roo_time::Uptime::Now() + roo_time::Millis(100);
  const alarm_id_t alarm = add_alarm_in_ms(5, PostRecords, &posts, false);
  ASSERT_GT(alarm, 0);
  do {
    posts.event.wait_until(deadline);
  } while (posts.queue.size() != 3 && roo_time::Uptime::Now() < deadline);
  cancel_alarm(alarm);
  ASSERT_LT(roo_time::Uptime::Now(), deadline);
  EXPECT_TRUE(posts.in_irq);
}

// Verifies a waiting loop is woken by a timer IRQ and the Pico ring retains
// order across the end of the array and rejects only excess records.
TEST(IsrPico, WakesAndWrapsTheRing) {
  Posts posts;
  ASSERT_NO_FATAL_FAILURE(WaitForRecords(posts));
  uint32_t record = 0;
  ASSERT_TRUE(posts.queue.try_pop(record));
  EXPECT_EQ(record, 1u);
  ASSERT_TRUE(posts.queue.try_pop(record));
  EXPECT_EQ(record, 2u);
  ASSERT_NO_FATAL_FAILURE(WaitForRecords(posts));
  for (uint32_t expected : {3, 5, 6}) {
    ASSERT_TRUE(posts.queue.try_pop(record));
    EXPECT_EQ(record, expected);
  }
  EXPECT_FALSE(posts.queue.try_pop(record));
  EXPECT_EQ(record, 6u);
  EXPECT_EQ(posts.rejected, 3u);
}

int64_t RawEvent(alarm_id_t, void* argument) {
  *static_cast<bool*>(argument) = true;
  __sev();
  return 0;
}

// Verifies a raw processor event can return control without a library signal.
TEST(IsrPico, WakesOnRawEvent) {
  roo::isr_event event;
  // Consume any latched events from earlier tests before parking.
  __sev();
  __wfe();
  const auto deadline = roo_time::Uptime::Now() + roo_time::Millis(100);
  bool raw_event = false;
  const alarm_id_t alarm = add_alarm_in_ms(5, RawEvent, &raw_event, false);
  ASSERT_GT(alarm, 0);
  do {
    event.wait_until(deadline);
  } while (!raw_event && roo_time::Uptime::Now() < deadline);
  cancel_alarm(alarm);
  EXPECT_TRUE(raw_event);
  EXPECT_LT(roo_time::Uptime::Now(), deadline);
}

// Verifies the pending bit survives a consumed hardware event and a later
// unsignaled wait advances to its absolute deadline.
TEST(IsrPico, RetainsSignalAndParksUntilDeadline) {
  roo::isr_event event;
  event.signal();
  __wfe();
  const auto start = roo_time::Uptime::Now();
  const auto deadline = start + roo_time::Millis(100);
  event.wait_until(deadline);
  EXPECT_EQ(roo_time::Uptime::Now(), start);
  while (roo_time::Uptime::Now() < deadline) event.wait_until(deadline);
  EXPECT_EQ(roo_time::Uptime::Now(), deadline);
}

}  // namespace
