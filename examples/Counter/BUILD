load("@roo_testing//roo_testing/emulation:arduino.bzl", "roo_arduino_example")

roo_arduino_example(
    name = "Counter",
    sketch = "Counter.ino",
    target_compatible_with = ["@roo_testing//roo_testing/platforms:freertos"],
    deps = ["//:roo_threads"],
)
