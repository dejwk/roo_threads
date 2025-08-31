#include "Arduino.h"
#include "roo_threads/mutex.h"
#include "roo_threads/thread.h"

class Counter {
 public:
  Counter() : val_(0), mutex_() {}

  void increment() {
    roo::lock_guard<roo::mutex> lock(mutex_);
    ++val_;
  }

  int get() const {
    roo::lock_guard<roo::mutex> lock(mutex_);
    return val_;
  }

 private:
  int val_;
  mutable roo::mutex mutex_;
};

void run(Counter* counter, size_t steps) {
  for (size_t i = 0; i < steps; i++) {
    counter->increment();
    // Yield, to increase the likelihood of race condidtions if synchronization
    // were to be disabled.
    roo::this_thread::yield();
  }
}

void setup() { Serial.begin(115200); }

void loop() {
  Counter counter;
  roo::thread t1(run, &counter, 10000);
  roo::thread t2(run, &counter, 8000);
  roo::thread t3(run, &counter, 12000);
  roo::thread t4(run, &counter, 15000);
  roo::thread t5(run, &counter, 5000);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  int result = counter.get();
  bool ok = (result == 50000);
  Serial.printf("Total count: %d %s\n", result, ok ? "(OK)" : "(Bad!)");
  delay(1000);
}
