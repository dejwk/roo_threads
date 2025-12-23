#pragma once

#include <functional>
#include <memory>
#include <utility>

#include "roo_threads/impl/resolve.h"

#ifdef ROO_THREADS_USE_FREERTOS

namespace roo_threads {
namespace freertos {

template <size_t... Indexes>
struct IndexTuple {
  typedef IndexTuple<Indexes..., sizeof...(Indexes)> next;
};

template <std::size_t N>
struct BuildIndexTuple {
  typedef typename BuildIndexTuple<N - 1>::type::next type;
};

template <>
struct BuildIndexTuple<0> {
  typedef IndexTuple<> type;
};

template <typename Tuple>
struct Invoker {
  Tuple tuple;

  template <size_t Index>
  static std::tuple_element_t<Index, Tuple>&& DeclVal();

  template <size_t... Ind>
  auto invoke(IndexTuple<Ind...>) noexcept(
      noexcept(std::invoke(DeclVal<Ind>()...)))
      -> decltype(std::invoke(DeclVal<Ind>()...)) {
    return std::invoke(std::get<Ind>(std::move(tuple))...);
  }

  using Indices = typename BuildIndexTuple<std::tuple_size<Tuple>::value>::type;

  auto operator()() noexcept(
      noexcept(std::declval<Invoker&>().invoke(Indices())))
      -> decltype(std::declval<Invoker&>().invoke(Indices())) {
    return invoke(Indices());
  }
};

template <typename... Tp>
using DecayedTuple = std::tuple<typename std::decay<Tp>::type...>;

template <typename Callable, typename... Args>
inline Invoker<DecayedTuple<Callable, Args...>> MakeInvoker(Callable&& callable,
                                                            Args&&... args) {
  return {DecayedTuple<Callable, Args...>{std::forward<Callable>(callable),
                                          std::forward<Args>(args)...}};
}

class VirtualCallable {
 public:
  virtual ~VirtualCallable() = default;
  virtual void call() = 0;
};

template <typename Callable>
class DynamicCallable : public VirtualCallable {
 public:
  DynamicCallable(Callable&& callable)
      : callable_(std::forward<Callable>(callable)) {}

  void call() override { callable_(); }

 private:
  Callable callable_;
};

template <typename Callable>
static std::unique_ptr<VirtualCallable> MakeDynamicCallable(
    Callable&& callable) {
  return std::unique_ptr<VirtualCallable>{
      new DynamicCallable<Callable>{std::forward<Callable>(callable)}};
}

// Given the callable and the arguments that should be bound to it, returns
// an instance of the VirtualCallable that invokes that callable and calls the
// arguments, using perfect forwarding and pass-by-reference.
template <typename Callable, typename... Args>
static std::unique_ptr<VirtualCallable> MakeDynamicCallableWithArgs(
    Callable&& callable, Args&&... args) {
  return MakeDynamicCallable(MakeInvoker(std::forward<Callable>(callable),
                                         std::forward<Args>(args)...));
}

}  // namespace freertos
}  // namespace roo_threads

#endif  // ROO_THREADS_USE_FREERTOS