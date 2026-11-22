#include <csignal>
#include <cstdint>

#include "gtest/gtest.h"
#include "roo_testing/interrupts/interrupt_controller.h"
#include "roo_threads.h"
#include "roo_threads/isr_critical_section.h"

namespace {

struct Record {
  uint32_t value;
  uint32_t check;

  Record* operator&() = delete;
  const Record* operator&() const = delete;
};

struct InterruptState {
  roo::isr_queue<Record, 3> queue;
  roo::isr_event event;
  roo::isr_critical_section lock;
  volatile sig_atomic_t posted = 0;
  volatile sig_atomic_t rejected = 0;
};

void PostRecords(void* argument) {
  auto& state = *static_cast<InterruptState*>(argument);
  {
    using Guard = roo::isr_lock_guard;
    Guard lock(state.lock, Guard::Context::kIsr);
    state.posted = xPortInIsrContext() == pdTRUE ? 1 : -1;
  }
  for (uint32_t v = 1; v <= 4; ++v) {
    if (!state.queue.try_push_from_isr(Record{v, ~v})) {
      state.rejected = state.rejected + 1;
    }
  }
  state.event.signal_from_isr();
}

roo_testing::InterruptHandle Register(InterruptState& state) {
  roo_testing::InterruptHandle handle;
  EXPECT_EQ(roo_testing::registerInterrupt(PostRecords, &state, true, &handle),
            roo_testing::InterruptRegistrationResult::kRegistered);
  return handle;
}

// Verifies that an actual emulated ISR can wake a blocked task and that each
// accepted record survives until consumed in order.
TEST(IsrFreeRtos, WakesAndDrainsRecords) {
  InterruptState state;
  const roo_testing::InterruptHandle handle = Register(state);
  ASSERT_TRUE(handle.isValid());

  const roo_time::Uptime deadline =
      roo_time::Uptime::Now() + roo_time::Seconds(2);
  roo::thread producer([handle] {
    roo::this_thread::sleep_for(roo_time::Millis(20));
    EXPECT_TRUE(roo_testing::setInterruptPending(handle));
  });
  state.event.wait_until(deadline);
  EXPECT_LT(roo_time::Uptime::Now(), deadline);
  producer.join();
  EXPECT_TRUE(roo_testing::unregisterInterrupt(handle));
  {
    roo::isr_lock_guard lock(state.lock);
    EXPECT_EQ(state.posted, 1);
  }
  EXPECT_EQ(state.rejected, 1);
  EXPECT_EQ(state.queue.size(), 3u);
  Record record{};
  for (uint32_t v = 1; v <= 3; ++v) {
    ASSERT_TRUE(state.queue.try_pop(record));
    EXPECT_EQ(record.value, v);
    EXPECT_EQ(record.check, ~v);
  }
  EXPECT_FALSE(state.queue.try_pop(record));
  EXPECT_EQ(record.value, 3u);
  EXPECT_EQ(state.queue.size(), 0u);
}

// Verifies the public task guard defers an interrupt until its scope ends.
TEST(IsrFreeRtos, TaskGuardDefersIsrDelivery) {
  InterruptState state;
  const roo_testing::InterruptHandle handle = Register(state);
  ASSERT_TRUE(handle.isValid());

  bool pending;
  sig_atomic_t posted_while_guarded;
  {
    roo::isr_lock_guard guard(state.lock);
    pending = roo_testing::setInterruptPending(handle);
    posted_while_guarded = state.posted;
  }

  const roo_time::Uptime deadline =
      roo_time::Uptime::Now() + roo_time::Seconds(2);
  state.event.wait_until(deadline);
  EXPECT_TRUE(roo_testing::unregisterInterrupt(handle));
  EXPECT_TRUE(pending);
  EXPECT_EQ(posted_while_guarded, 0);
  EXPECT_EQ(state.posted, 1);
}

// Verifies a signal before the wait is retained, repeated signals coalesce,
// and an already-expired deadline returns immediately.
TEST(IsrFreeRtos, RetainsOneSignalAndTimesOut) {
  roo::isr_event event;
  event.signal();
  event.signal();
  const roo_time::Uptime deadline =
      roo_time::Uptime::Now() + roo_time::Millis(100);
  event.wait_until(deadline);
  EXPECT_LT(roo_time::Uptime::Now(), deadline);
  event.wait_until(deadline);
  // A tick-based wait can end within one tick of the requested deadline.
  EXPECT_GE(roo_time::Uptime::Now() + roo_time::Millis(portTICK_PERIOD_MS),
            deadline);
  event.wait_until(roo_time::Uptime::Start());
}

}  // namespace
