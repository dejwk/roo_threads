#pragma once

#include "roo_threads/doc/mutex_api.h"
#include "roo_time.h"

namespace roo_threads {
namespace doc {

/// @ingroup roo_threads_api_condition_variable
/// @brief Status returned from timed wait operations.
enum class cv_status { no_timeout, timeout };

/// @ingroup roo_threads_api_condition_variable
/// @brief Synchronization primitive for blocking and waking threads.
class condition_variable {
 public:
  /// @brief Constructs a condition variable.
  condition_variable() noexcept;

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  /// @brief Waits until notified; may wake spuriously.
  /// @param lock lock associated with the condition variable.
  void wait(unique_lock<mutex>& lock) noexcept;

  /// @brief Waits until predicate becomes true.
  /// @param lock lock associated with the condition variable.
  /// @param pred predicate to evaluate after wakeups.
  template <class Predicate>
  void wait(unique_lock<mutex>& lock, Predicate pred);

  /// @brief Wakes one waiting thread.
  void notify_one() noexcept;

  /// @brief Wakes all waiting threads.
  void notify_all() noexcept;

  /// @brief Waits until notified or deadline is reached.
  /// @param lock lock associated with the condition variable.
  /// @param when absolute timeout point.
  /// @return timeout status.
  cv_status wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when);

  /// @brief Waits until predicate becomes true or deadline is reached.
  /// @param lock lock associated with the condition variable.
  /// @param when absolute timeout point.
  /// @param p predicate to evaluate after wakeups.
  /// @return true if predicate became true.
  template <typename Predicate>
  bool wait_until(unique_lock<mutex>& lock, const roo_time::Uptime& when,
                  Predicate p);

  /// @brief Waits until notified or duration elapses.
  /// @param lock lock associated with the condition variable.
  /// @param duration relative timeout duration.
  /// @return timeout status.
  cv_status wait_for(unique_lock<mutex>& lock,
                     const roo_time::Duration& duration);

  /// @brief Waits until predicate becomes true or duration elapses.
  /// @param lock lock associated with the condition variable.
  /// @param duration relative timeout duration.
  /// @param p predicate to evaluate after wakeups.
  /// @return true if predicate became true.
  template <typename Predicate>
  bool wait_for(unique_lock<mutex>& lock, const roo_time::Duration& duration,
                Predicate p);
};

}  // namespace doc
}  // namespace roo_threads
