#include <Arduino.h>
#include <roo_threads.h>

#if ROO_THREADS_HAS_ISR_SUPPORT
#if defined(ESP32) && CONFIG_ARDUINO_ISR_IRAM
#error This example requires Arduino GPIO handlers registered without ESP_INTR_FLAG_IRAM
#endif

namespace {

constexpr int kInputPin = 4;
roo::isr_queue<uint8_t, 8> gpio_requests;
roo::isr_event gpio_event;

void OnGpioEdge() {
  if (gpio_requests.try_push_from_isr(kInputPin)) {
    gpio_event.signal_from_isr();
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kInputPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(kInputPin), OnGpioEdge, FALLING);
}

void loop() {
  uint8_t pin;
  while (gpio_requests.try_pop(pin)) {
    Serial.print("Falling edge on GPIO ");
    Serial.println(pin);
  }
  gpio_event.wait();
}

#else

void setup() {}
void loop() {}

#endif
