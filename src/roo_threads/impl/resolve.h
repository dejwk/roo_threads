#pragma once

#ifndef ROO_THREADS_SINGLETHREADED
#if (defined ROO_TESTING)
#define ROO_THREADS_USE_ROO_TESTING
#elif (defined ESP32)
#define ROO_THREADS_USE_CPPSTD
#else
#define ROO_THREADS_SINGLETHREADED
#endif
#endif
