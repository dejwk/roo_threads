#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

#ifndef ROO_THREADS_FREERTOS_USE_OPTIMIZED_CONDITION_VARIABLE
#define ROO_THREADS_FREERTOS_USE_OPTIMIZED_CONDITION_VARIABLE 1
#endif

#if ROO_THREADS_FREERTOS_USE_OPTIMIZED_CONDITION_VARIABLE == 1

#include "roo_threads/impl/freertos/condition_variable_opt.h"
namespace roo_threads {
namespace freertos {

using condition_variable = condition_variable_opt;

}  // namespace freertos
}  // namespace roo_threads

#else

#include "roo_threads/impl/freertos/condition_variable_classic.h"
namespace roo_threads {
namespace freertos {
using condition_variable = condition_variable_classic;

}  // namespace freertos
}  // namespace roo_threads

#endif

#endif  // ROO_THREADS_USE_FREERTOS