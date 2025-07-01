#pragma once

#if (defined ROO_TESTING)
#include "roo_testing/sys/condition_variable.h"

namespace roo {

using cv_status = roo_testing::cv_status;
using condition_variable = roo_testing::condition_variable;

}

#elif (defined ESP32)
#include <condition_variable>

namespace roo {

using cv_status = std::cv_status;
using condition_variable = std::condition_variable;

}

#endif
