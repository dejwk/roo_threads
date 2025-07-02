This library provides a library of concurrency primitives (thread, mutex, condition variable, locks, etc.), in the 'roo::' namespace. These generally mimic C++ standard, except that they do not depend on `<chrono>`, but on roo_time.

For ESP32, it uses the standard C++ library underneath.

For other microcontrollers, and by default, the library compiles in a 'single-threaded' mode, in which the functionality to start new threads is not available, but a lot of mutex/lock functionality
is implemented (usually as no-op). Because of that, it OK to implement classes that use mutexes and locks, so that they are thread-safe on multi-threaded platforms, but still compile and work on single-threaded platforms.

Additinally, this library works correctly with the roo_testing environment, allowing to emulate multi-threaded ESP32 programs in the Linux environment.
