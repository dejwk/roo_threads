#include "roo_threads/impl/resolve.h"

// Verifies that a future emulated RP2040 FreeRTOS runtime retains its backend.
#ifndef ROO_THREADS_USE_FREERTOS
#error RP2040 FreeRTOS identity selected the wrong Roo backend
#endif
#ifdef ROO_THREADS_SINGLETHREADED
#error RP2040 identity incorrectly forced the single-threaded backend
#endif
