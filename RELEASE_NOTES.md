# [roo_threads 1.2.7](https://github.com/dejwk/roo_threads/releases/tag/1.2.7)

Published 2026-08-29.

### What’s new

- All bundled examples can now be built and run with the Roo emulator.
- Added Bazel build targets for the ProducerConsumer, Testing, ThreadAttributes, and Threads examples.

### Compatibility

- No API changes or breaking changes.
- This release improves example validation and host-side development workflows.

---

# [roo_threads 1.2.6](https://github.com/dejwk/roo_threads/releases/tag/1.2.6)

Published 2026-08-21.

# roo_threads 1.2.6

## Fixed

- Prevented overflow in long FreeRTOS timed waits by clamping delays to a safe, portable maximum.
- Made condition-variable timing tests deterministic, improving reliability across host profiles.

## Changed

- Updated host emulation to `roo_testing` 2.0, with support for both Arduino ESP32 and ESP-IDF profiles.
- Added a runnable Counter example:

  ```sh
  bazel run //examples/Counter:Counter
  ```

- Updated dependencies to `roo_testing` 2.0.1 and `roo_time` 1.4.6.
- Modernized Bazel and CI configuration, including centralized AddressSanitizer support.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.2.5...1.2.6

---

# [roo_threads 1.2.5](https://github.com/dejwk/roo_threads/releases/tag/1.2.5)

Published 2026-03-18.

Fix: linker errors on single-threaded environments.

---

# [roo_threads 1.2.4](https://github.com/dejwk/roo_threads/releases/tag/1.2.4)

Published 2026-03-18.

Fixing compilation issues on ESP8266.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.2.3...1.2.4

---

# [roo_threads 1.2.3](https://github.com/dejwk/roo_threads/releases/tag/1.2.3)

Published 2026-02-25.

* Added doxygen comments for the API documentation.
* Updated dependencies, which fixes a problem with crashes under libunwind.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.2.2...1.2.3

---

# [roo_threads 1.2.2](https://github.com/dejwk/roo_threads/releases/tag/1.2.2)

Published 2026-02-24.

Updated dependencies and fixed tests when running with libunwind.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.2.1...1.2.2

---

# [roo_threads 1.2.1](https://github.com/dejwk/roo_threads/releases/tag/1.2.1)

Published 2026-01-25.

* Added support for 32-bit stack sizes (i.e. > 64 KB),
* Minor memory optimization: don't wait for 'join' to release memory used by startup parameters.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.2.0...1.2.1

---

# [roo_threads 1.2.0](https://github.com/dejwk/roo_threads/releases/tag/1.2.0)

Published 2026-01-06.

* New features: latch, atomics,
* Added support for esp-idf,
* Added support for RP2040 SMP,
* Added a general implementation for FreeRTOS that is expected to work with other FreeRTOS-based platforms,
* Updated examples,
* A few bugfixes,
* Added semaphore tests,
* Minor usability tweaks.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.1.6...1.2.0

---

# [roo_threads 1.1.6](https://github.com/dejwk/roo_threads/releases/tag/1.1.6)

Published 2025-11-12.

Adding basic support for semaphores.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.1.5...1.1.6

---

# [roo_threads 1.1.5](https://github.com/dejwk/roo_threads/releases/tag/1.1.5)

Published 2025-10-30.

Picking up updated testing dependencies.

---

# [roo_threads 1.1.4](https://github.com/dejwk/roo_threads/releases/tag/1.1.4)

Published 2025-10-30.

Updated continuous integration, and .gitignore.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.1.3...1.1.4

---

# [roo_threads 1.1.3](https://github.com/dejwk/roo_threads/releases/tag/1.1.3)

Published 2025-10-05.

* Overflow protection in conditional variable wait with very long timeouts.
* Updated roo_testing and roo_time dependencies.

**Full Changelog**: https://github.com/dejwk/roo_threads/compare/1.1.2...1.1.3

---

# [roo_threads 1.1.2](https://github.com/dejwk/roo_threads/releases/tag/1.1.2)

Published 2025-09-26.

* Compilation fix: make it possible to pass thread attributes by value.
* Fix: make it possible to create threads from a static initializer on ESP32-C3.

---

# [roo_threads 1.1.1](https://github.com/dejwk/roo_threads/releases/tag/1.1.1)

Published 2025-09-23.

Adding overflow protection in condition_variable.wait_for().


---

# [roo_threads 1.1.0](https://github.com/dejwk/roo_threads/releases/tag/1.1.0)

Published 2025-09-12.

Some major improvements:
* adding the ability to set thread attributes on creation;
* better compatibility with the native tasks on ESP32;
* added unit tests and CI/CD.


---

# [roo_threads 1.0.1](https://github.com/dejwk/roo_threads/releases/tag/1.0.1)

Published 2025-07-31.

Adding main header file for compatibility with the Arduino IDE.

---

# [roo_threads 1.0.0](https://github.com/dejwk/roo_threads/releases/tag/1.0.0)

Published 2025-07-02.

Initial release.

---

