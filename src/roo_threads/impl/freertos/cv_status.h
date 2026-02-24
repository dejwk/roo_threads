#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

namespace roo_threads {
namespace freertos {

/// @ingroup roo_threads_api_condition_variable
/// @copydoc roo_threads::doc::cv_status
enum class cv_status { no_timeout, timeout };

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS
