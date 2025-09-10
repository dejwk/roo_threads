// This example demonstrates how to use condition variables and mutexes to
// implement a blocking producer-consumer queue with bound capacity.

#include <deque>

#include "Arduino.h"
#include "roo_threads/condition_variable.h"
#include "roo_threads/mutex.h"
#include "roo_threads/thread.h"

class Queue {
 public:
  struct Item {
    int tag;  // Identifies the producer thread.
    int value;
  };

  Queue(size_t max_capacity) : deque_(), max_capacity_(max_capacity) {}

  void put(Item item) {
    roo::unique_lock<roo::mutex> guard(mutex_);
    while (deque_.size() == max_capacity_) {
      nonfull_.wait(guard);
    }
    deque_.push_back(std::move(item));
    nonempty_.notify_all();
  }

  Item take() {
    roo::unique_lock<roo::mutex> guard(mutex_);
    while (deque_.empty()) {
      nonempty_.wait(guard);
    }
    Item result = std::move(deque_.front());
    deque_.pop_front();
    nonfull_.notify_all();
    return result;
  }

 private:
  std::deque<Item> deque_;
  size_t max_capacity_;
  mutable roo::mutex mutex_;
  roo::condition_variable nonfull_;
  roo::condition_variable nonempty_;
};

void run(Queue* queue, int tag, roo_time::Interval interval) {
  int val = 0;
  while (true) {
    queue->put(Queue::Item{tag, val});
    Serial.printf("Published: %d.%d\n", tag, val);
    ++val;
    roo::this_thread::sleep_for(interval);
  }
}

roo::thread producer1;
roo::thread producer2;
Queue queue(100);

void setup() {
  Serial.begin(115200);
  // Start two producer threads, with slightly different frequency.
  producer1 = roo::thread(run, &queue, 1, roo_time::Millis(200));
  producer2 = roo::thread(run, &queue, 2, roo_time::Millis(250));
}

void loop() {
  // The consumer takes items from the queue as soon as they appear.
  Queue::Item item = queue.take();
  Serial.printf("                       Consumed: %d.%d\n", item.tag,
                item.value);
}
