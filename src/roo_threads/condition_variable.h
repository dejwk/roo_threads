#pragma once

#include "roo_threads/impl/resolve.h"

#if (defined ROO_THREADS_USE_CPPSTD)

#include "roo_threads/impl/cppstd/condition_variable.h"

namespace roo {

using cv_status = ::roo_threads::cppstd::cv_status;
using condition_variable = ::roo_threads::cppstd::condition_variable;

}  // namespace roo

#elif (defined ROO_THREADS_USE_ROO_TESTING)

#include "roo_threads/impl/roo_testing/condition_variable.h"

namespace roo {

using cv_status = ::roo_threads::roo_testing::cv_status;
using condition_variable = ::roo_threads::roo_testing::condition_variable;

}  // namespace roo

#elif (defined ROO_THREADS_SINGLETHREADED)

#include "roo_threads/impl/singlethreaded/condition_variable.h"

namespace roo {

using cv_status = ::roo_threads::singlethreaded::cv_status;
using condition_variable = ::roo_threads::singlethreaded::condition_variable;

}  // namespace roo

#endif
