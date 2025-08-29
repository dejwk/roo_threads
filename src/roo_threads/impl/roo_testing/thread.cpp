#include "roo_threads/impl/roo_testing/thread.h"

#ifdef ROO_THREADS_USE_ROO_TESTING

namespace roo_threads {
namespace roo_testing {

::roo_testing::thread::attributes make_attrs(const thread::attributes& attrs) {
  ::roo_testing::thread::attributes result;
  if (attrs.stack_size_ > 0) {
    result.set_stack_size(attrs.stack_size_);
  }
  if (attrs.priority_ >= 0) {
    result.set_priority(attrs.priority_);
  }
  if (attrs.name_ != nullptr) {
    result.set_name(attrs.name_);
  }
  return result;
}

}  // namespace roo_testing
}  // namespace roo_threads

#endif