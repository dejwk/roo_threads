// Thread attributes are a non-portable extension. This example demonstrates
// how to use them to customize thread name, priority, and stack size, on
// Espressif SOCs (e.g. ESP32, ESP32-S2, ESP32-C3, etc).

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "roo_threads/thread.h"

void run(int tid) {
  // Threads are mapped onto FreeRTOS tasks. You can use FeeRTOS task APIs to
  // interact with them in a platform-specific way.
  TaskHandle_t handle = xTaskGetCurrentTaskHandle();
  Serial.printf("%d (%p): %s, priority: %d, free stack: %d\n", tid, handle,
                pcTaskGetName(handle), uxTaskPriorityGet(handle),
                uxTaskGetStackHighWaterMark(handle) * sizeof(StackType_t));
}

void setup() { Serial.begin(115200); }

void loop() {
  // Default attributes,
  roo::thread::attributes attrs1;
  roo::thread t1(attrs1, run, 1);

  // Override thread name.
  roo::thread::attributes attrs2;
  attrs2.set_name("Antonio");
  roo::thread t2(attrs2, run, 2);

  // Override other attributes as well.
  roo::thread::attributes attrs3;
  attrs3.set_name("Richard");
  attrs3.set_priority(1);
  attrs3.set_stack_size(8192);  // 8 KB.
  roo::thread t3(attrs3, run, 3);

  t1.join();
  t2.join();
  t3.join();
  Serial.println();

  roo::this_thread::sleep_for(roo_time::Seconds(2));
}
