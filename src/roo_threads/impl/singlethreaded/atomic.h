#pragma once

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_SINGLETHREADED

enum memory_order : int {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
};

template <typename T>
struct atomic {
  using value_type = T;

 public:
  atomic() = default;
  ~atomic() noexcept = default;
  atomic(const atomic&) = delete;
  atomic& operator=(const atomic&) = delete;
  atomic& operator=(const atomic&) volatile = delete;

  constexpr atomic(T value) noexcept : value_(value) {}

  operator T() const noexcept { return value_; }

  operator T() const volatile noexcept { return value_; }

  T operator=(T value) noexcept {
    value_ = value;
    return value;
  }

  T operator=(T value) volatile noexcept {
    value_ = value;
    return value;
  }

  bool is_lock_free() const noexcept {
    return true;
  }

  bool is_lock_free() const volatile noexcept {
    return true;
  }

  void store(T value, memory_order m = memory_order_seq_cst) noexcept {
    value_ = value;
  }

  void store(T value,
             memory_order m = memory_order_seq_cst) volatile noexcept {
    value_ = value;
  }

  T load(memory_order m = memory_order_seq_cst) const noexcept {
    return value_;
  }

  T load(memory_order m = memory_order_seq_cst) const volatile noexcept {
    return value_;
  }

  T exchange(T value, memory_order m = memory_order_seq_cst) noexcept {
    T tmp = value_;
    value_ = value;
    return tmp;
  }

 private:
  T value_;
};

#endif  // ROO_THREADS_SINGLETHREADED