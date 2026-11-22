# ISR Queues and Wake Events

## Objective

Let interrupt handlers hand work to a waiting task or main loop through
`roo_threads` on FreeRTOS and bare-metal RP2040.

## Motivation

A GPIO or timer handler often needs to record work for later. A bare-metal Pico
loop can use a processor event to wake up. A FreeRTOS application needs to unblock
a task. Libraries that support both currently have to carry those platform
details themselves.

## Background

An interrupt service routine (ISR) can preempt the code that consumes its
requests, even on a single core. A mutex taken by the interrupted code is
therefore unsuitable for the handoff. The single-threaded `roo::mutex` is a
no-op. The current backend selector chooses the implementation of ordinary
thread primitives, including the C++ standard library on ESP32 when requested.
ISO C++ specifies rules for C++ signal handlers and leaves hardware interrupt
handling to the platform.
[Single-threaded mutex](../src/roo_threads/impl/singlethreaded/mutex.h)
· [Backend selector](../src/roo_threads/impl/resolve.h)
· [C++ signal handlers](https://eel.is/c++draft/support.signal)

## Requirements

- An ISR can submit a small value without allocating, sleeping, or taking a
  task mutex. Capacity is fixed before interrupts are enabled. Saturation is
  visible to the producer.
- Accepted values reach a single consumer in queue order. The consumer sees
  the copied value in full.
- A task or ISR can wake the consumer. A signal between the consumer's last
  check for work and its wait is retained. The consumer can also wait with an
  absolute uptime deadline.
- Port selection and yield requests stay inside `roo_threads`. Callers can
  determine at compile time whether both operations are available.

## Design Overview

An **ISR queue** stores fixed-size records in submission order. It preserves
individual requests through bursts and reports when storage is full. An **ISR
event** holds one pending wakeup. The consumer can wait once and then inspect
its own work and deadline predicates. Many records can share one wakeup.
Keeping the event independent also lets normal task code wake a consumer when
its next deadline changes without occupying a queue slot.

For example, a timer handler copies a sample index into an eight-slot queue
and signals the event. Three interrupts before the main loop resumes leave
three indices and one pending wakeup. The loop processes the indices in order.
On a full queue the handler can count the dropped sample or let the peripheral
retain it for later. The sample storage itself needs a lifetime and
synchronization protocol appropriate to the device.

## Design Details

### Backend selection

Resolve the ISR backend at compile time using the configured application
runtime and its available APIs. `ROO_THREADS_HAS_ISR_SUPPORT` is always
defined and is `1` only when both types have a supported implementation.

| Build | ISR backend |
| --- | --- |
| ESP-IDF or Arduino-ESP32 | FreeRTOS, including ESP32 builds with `ROO_THREADS_ESP32_PREFER_CPPSTD=1`. |
| Arduino-Pico with `__FREERTOS` | FreeRTOS. |
| Bare-metal Arduino-Pico on RP2040 | Pico, with SDK 2.1 or newer and the default alarm pool enabled. |
| Native Pico SDK on RP2040 | Pico, with the same SDK and alarm pool requirements and an explicit bare-metal target setting. |
| `roo_testing` with `ROO_TESTING_RUNTIME_FREERTOS=1` | Emulated FreeRTOS. |
| `roo_testing` with `ROO_TESTING_RUNTIME_BARE_METAL=1` and Pico profile identity | Emulated Pico. |
| Other builds | Capability `0`. Clients can use it to omit ISR-specific entry points. |

Check emulated runtime identity before physical-device flags. The emulated
Pico path requires a matching runtime and SDK profile. On device, check
Arduino-Pico's `__FREERTOS` first and exclude SDK-only FreeRTOS targets from
the bare-metal branch using `LIB_FREERTOS_KERNEL`. Native Pico SDK targets
select the bare-metal backend with `ROO_THREADS_PICO_SDK_BARE_METAL=1` plus
`PICO_ON_DEVICE=1` and the RP2040 identity. Set the runtime flag on the build
target so it reaches the library and its callers. Native Pico SDK FreeRTOS
builds and bare-metal RP2350 builds have capability `0` in this release.

`ROO_THREADS_SINGLETHREADED` selects the ordinary thread backend. An
application using that override on FreeRTOS can use the ISR primitives while
the kernel is running. Each port advertises support when both primitives and
the corresponding critical-section helper are available.
[Arduino-Pico FreeRTOS configuration](../README.md#raspberry-pi-pico)
· [roo_testing runtime contract](https://github.com/dejwk/roo_testing/blob/main/roo_testing/frameworks/environment/runtime_profile_contract.c)
· [Native Pico SDK FreeRTOS build definitions](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/ThirdParty/GCC/RP2040/library.cmake)

### ISR calling requirements

On FreeRTOS, the caller's interrupt priority must permit kernel API calls.
On ESP32, a cache-disabled handler can use these APIs only when the entire
call path and all accessed data reside in internal memory. This includes the
backend, the queue or event object, and any data copied by the ISR. The
capability macro reports backend availability. The caller is responsible for
verifying placement in the linked firmware.
[ESP-IDF interrupt allocation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/intr_alloc.html#iram-safe-interrupt-handlers)

### Critical-section helper

Provide a backend helper for short critical sections that exclude interrupt
and task or loop access to the same data, including access from the other
core on supported multicore builds. Keep the helper in the implementation
headers. The scheduler uses it for its own fixed slots, which must stay
occupied while work is eligible for dispatch. The public FIFO remains
available for applications that consume records in order.

On ESP-IDF and Arduino-ESP32, use the port's task and ISR critical-section
operations with one mux per protected object. On the Arduino-Pico FreeRTOS
port and the FreeRTOS emulator, use the kernel's task and ISR critical-section
operations. On single-core emulated Pico, save and restore the local IRQ mask.

On bare-metal RP2040, use the SDK's shared stripe at
`PICO_SPINLOCK_ID_STRIPED_FIRST`. Get the lock with `spin_lock_instance()`
and acquire and release it with local IRQ save and restore. Using a fixed
stripe lets objects be constructed independently on either core without
touching the SDK's round-robin allocator or storing a lock per object.
Keep critical sections short and release them before signaling, waiting,
or entering another critical section that uses the shared stripe.
[Companion scheduler storage design](../../roo_scheduler/docs/isr-task-scheduling.md#submission-and-storage)

### Queue semantics and storage

A successful push copies the bytes of one trivially copyable value into
preallocated storage. A failed push leaves the queue intact and means it was
full at the operation's synchronization point. The queue supports concurrent
ISR producers. The owner serializes consumer-side pops and can read the
current size from task or loop context. A size read is a snapshot. The
platform primitive provides the synchronization for the record copy.
Borrowed objects referenced by a record remain the caller's responsibility.

On FreeRTOS, use `xQueueCreateStatic`, `xQueueSendFromISR`, and zero-timeout
`xQueueReceive`. Use the queue's task-context count operation for `size()`.
Keep the port-specific yield request inside the backend. The FreeRTOS
backend requires static allocation support. Advertise the capability only
for FreeRTOS configurations that provide it.

On Pico, use a ring with a read index and count. Use the critical-section
helper for one record copy and its index update, or for one count read.
Release the queue lock before signaling the event.
[FreeRTOS ISR queue API](https://www.freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/04-xQueueSendFromISR)
· [Pico SDK synchronization](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#hardware_sync)

A queue with capacity `N` and record size `S` bytes reserves `N * S` bytes for
its records even when empty. It also holds native queue metadata on FreeRTOS
or two indices on Pico. Each push or pop copies one record and does constant
additional queue work. Copy cost grows with `S`. On RP2040, any operation
using the shared stripe can spin while the other core holds it. The amount
of queue work per call is bounded.

### Wake event semantics

A signal sets a single pending wakeup. A wait consumes a pending wakeup and
returns immediately. With no pending wakeup it parks once, subject to the
requested deadline. A parked wait can also return early, so callers recheck
their predicates after every return. A wait with an expired deadline returns
immediately. Only one consumer waits on the event at a time.

On FreeRTOS, use a static binary semaphore. `signal()` gives from task context
and `signal_from_isr()` uses `xSemaphoreGiveFromISR` with the port's deferred
yield convention. A full semaphore already represents the wakeup. Convert
absolute deadlines with the existing tick rounding and range limits, and use
the untimed wait for an infinite deadline. Make the timeout conversion
available to the ISR backend independently of the ordinary thread backend.
[FreeRTOS ISR semaphore API](https://www.freertos.org/Documentation/02-Kernel/04-API-references/10-Semaphore-and-Mutexes/17-xSemaphoreGiveFromISR)
· [FreeRTOS timeout conversion](../src/roo_threads/impl/freertos/timeutil.h)

On Pico, protect a pending bit with the critical-section helper. Set the bit
before releasing the lock and issuing `SEV`. The consumer tests and clears
the bit under the lock. When the bit is clear, release the lock and use one
`WFE` or one `best_effort_wfe_or_timeout` for a finite deadline. A producer
after the bit check sets the bit and issues an event that wakes the consumer
or is already pending when it parks. SDK 2.1 retains an already-latched
event in the timed wait. SDK 2.0.0 could clear that event before sleeping.
One-shot waits also let a raw processor event return control to a main loop.
Keep SDK time conversion and processor instructions inside the backend.
[Pico SDK lock and timed-wait semantics](https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#lock_core)
· [Pico SDK timed-wait fix](https://github.com/raspberrypi/pico-sdk/issues/1812)

## Proposed API

Declare the types in `roo_threads/isr_queue.h` and `roo_threads/isr_event.h`.
Expose the capability from `roo_threads/isr_support.h`. The declarations below
show the public surface on a supported build.

```cpp
namespace roo {

/// Copies small ISR requests into fixed-capacity FIFO storage.
template <typename T, std::size_t Capacity>
class isr_queue {
 public:
  /// Creates an empty queue with Capacity slots prepared for ISR use.
  isr_queue() noexcept;
  isr_queue(const isr_queue&) = delete;
  isr_queue& operator=(const isr_queue&) = delete;

  /// Copies value into the queue from an ISR. Returns false when full.
  /// Follow the ISR calling requirements for priority and memory placement.
  bool try_push_from_isr(const T& value) noexcept;

  /// Copies and removes the oldest record in consumer context.
  /// Returns false and leaves value intact when empty.
  bool try_pop(T& value) noexcept;

  /// Returns the current record count in task or loop context.
  std::size_t size() const noexcept;

 private:
  // Backend metadata and inline storage for Capacity records.
};

/// Stores a pending wakeup for one waiting task or loop.
class isr_event {
 public:
  /// Creates an event with no pending wakeup.
  isr_event() noexcept;
  isr_event(const isr_event&) = delete;
  isr_event& operator=(const isr_event&) = delete;

  /// Records a wakeup from task or loop context.
  void signal() noexcept;

  /// Records a wakeup from an ISR and requests a port yield when needed.
  /// Follow the ISR calling requirements for priority and memory placement.
  void signal_from_isr() noexcept;

  /// Consumes a pending wakeup or waits once for an event.
  void wait() noexcept;

  /// Consumes a pending wakeup or waits once with an absolute deadline.
  void wait_until(roo_time::Uptime deadline) noexcept;

 private:
  // Native static semaphore, or a pending bit on Pico.
};

}  // namespace roo
```

Require positive capacity, trivially copyable `T`, and sizes representable
by the backend. Construct both objects before enabling the interrupt source.
Stop and synchronize all producers and the waiter before destroying them.

## Implementation Plan

Follow the repository's [C++ authoring instructions](../.github/instructions/general-cpp-code-authoring-instructions.md).

### Phase 1. FreeRTOS queue and wake event

**Proposed commit message** `Add ISR queue and wake event support on FreeRTOS`

Add the primitive headers, capability selector, and FreeRTOS implementations,
including the critical-section helper. Include a producer and consumer
example. Document ISR calling requirements alongside the public declarations.
Validate static storage, FIFO and saturation, signal retention before wait,
timeouts, and the port-specific yield path with an emulated interrupt.
Compile-check ESP32 with the C++ thread backend and Arduino-Pico with FreeRTOS.

### Phase 2. Bare-metal RP2040 queue and wake event

**Proposed commit message** `Add bare-metal Pico support for ISR queue and wake events`

Add the Pico implementations and put the SDK dependency on the `roo_threads`
target. Extend the example and document the Pico wake contract. Validate an
emulated alarm callback, signals before and during sleep, a raw `SEV`, and
the absolute deadline. On RP2040 hardware, post from both cores into the
same queue. Force a signal after the pending-bit check and before the timed
wait and verify the consumer returns before the old deadline. Verify the
waiter parks with no work. Add negative selection probes for generic
bare-metal, native SDK FreeRTOS, Pico SDK 2.0.0, and bare-metal RP2350.

## Testing Plan

Use the repository's existing [Bazel test profiles](../README.md#host-emulation)
for ESP32, Arduino-Pico with FreeRTOS, and bare-metal Pico. Extend the
[backend selection tests](../test/backend_selection_test.cpp) to cover the
ordinary backend overrides independently of ISR selection. Exercise both Pico
SDK and Arduino-Pico bare-metal builds. Use a physical Pico for cross-core
behavior, which the bare-metal emulator models on a single core.

## Caveats

On RP2040, a full default alarm pool can make a timed wait return early.
Callers recheck their work and deadline predicates after the return.

### Rejected Alternatives

#### Direct-to-task notifications

On FreeRTOS, a reserved task notification index can be a low-cost wake
signal. It couples the event to a specific task and requires reserving an
exclusive index. The optimized condition
variable already uses index zero. A binary semaphore gives each event
independent storage and permits a consumer to begin waiting after the first
signal.
[Optimized condition variable](../src/roo_threads/impl/freertos/condition_variable_opt.cpp)

#### Standard C++ synchronization primitives

Standard atomics can cover some record publication designs, and standard
semaphores can wake threads in task context. Hardware ISR guarantees and
the wake of an RTOS task or Pico loop still require platform support. The
backend selector makes that support explicit.

## Future Work

Enable bare-metal RP2350 when a device test shows an idle waiter parks after
checking the event and wakes for both library signals and raw `SEV`.
SDK software spin locks on RP2350 can set the calling core's event latch,
so the RP2040 wait sequence can keep an RP2350 loop active while idle.
[Pico SDK synchronization workaround](https://github.com/raspberrypi/pico-sdk/blob/master/src/common/pico_sync/include/pico/lock_core.h)
