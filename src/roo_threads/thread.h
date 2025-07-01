#pragma once

#if (defined ROO_TESTING)
#include "roo_testing/sys/thread.h"

namespace roo {

using thread = roo_testing::thread;

}

#elif (defined ESP32)
#include <thread>

namespace roo {

using thread = std::thread;

}

#endif