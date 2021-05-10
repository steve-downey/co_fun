// delay.h                                                            -*-C++-*-
#ifndef INCLUDED_DELAYASYNC
#define INCLUDED_DELAYASYNC

#include <optional>
#include <functional>
#include <atomic>
#include <mutex>
#include <future>
#include <iostream>

template <typename Value>
class DelayAsync {
  using Func = std::function<Value()>;
  template <typename Action>
  using isFuncConv = std::is_convertible<Action, Func>;

  std::shared_future<Value> future_;
  mutable std::atomic_int evaled_;

public:
  DelayAsync() = default;
  DelayAsync(const DelayAsync& rhs) : future_(rhs.future_) {
  }
  DelayAsync(Value const& value)
      : future_(std::async(std::launch::deferred, [value]() { return value; })),
        evaled_(true){};
  DelayAsync(Value&& value)
      : future_(std::async(std::launch::deferred,
                           [value = std::move(value)]() { return value; })),
        evaled_(true){};

  template <typename Action,
            typename = typename std::enable_if<isFuncConv<Action>::value>::type>
  DelayAsync(Action&& A)
      : future_(std::async(
            std::launch::deferred,
            [action = std::forward<Action>(A)]() { return action(); })),
        evaled_(false) {
  }

  ~DelayAsync() = default;

  Value const& get() const {
    evaled_ = true;
    return future_.get();
  }

  operator Value const&() const {
    evaled_ = true;
    return future_.get();
  }

  bool isForced() {
    return evaled_;
  }
};

template <typename Value>
Value const& force(DelayAsync<Value> const& delayAsync) {
  return delayAsync;
}

template <typename Value>
Value const& force(DelayAsync<Value>&& delayAsync) {
  return std::move(delayAsync);
}

template <typename F, typename... Args>
auto delayAsync(F&& f, Args&&... args) -> DelayAsync<decltype(f(args...))> {
  using Value = decltype(f(args...));
  return DelayAsync<Value>(
      [ args..., f_ = std::forward<F>(f) ]() { return f_(args...); });
}

#endif
