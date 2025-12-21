#include "Arduino.h"

#include "roo_threads/thread.h"

roo::thread t1;
roo::thread t2;
roo::thread t3;

void run(const char* name, roo_time::Duration freq) {
  while (true) {
    Serial.print(millis());
    Serial.print(": ");
    Serial.println(name);

    Serial.flush();
    roo::this_thread::sleep_for(freq);
    // Or, equivalently:
    // delay(freq.inMillisRoundedUp());
  }
}

void setup() {
  Serial.begin(115200);
  t1 = roo::thread(run, "Thread 1", roo_time::Millis(500));
  t2 = roo::thread(run, "Thread 2", roo_time::Millis(1200));
  t3 = roo::thread(run, "Thread 3", roo_time::Millis(2700));
}

void loop() {
  roo::this_thread::sleep_for(roo_time::Seconds(10));
    Serial.print(millis());
    Serial.print(": loop");
}
