// delay.h                                                            -*-C++-*-
#ifndef INCLUDED_DELAY
#define INCLUDED_DELAY

#include <optional>
#include <functional>
#include <atomic>
#include <mutex>

#include <iostream>

template <typename Value>
class Delay {
  using Func = std::function<Value()>;
  template <typename Action>
  using isFuncConv = std::is_convertible<Action, Func>;

  mutable Func func_;

  typedef typename std::aligned_storage<sizeof(Value),
                                        std::alignment_of<Value>::value>::type
      Storage;
  mutable Storage value_;
  mutable std::atomic_int evaled_;
  mutable std::mutex lock_;

  void setValue() const {
    std::unique_lock<std::mutex> guard(lock_);
    if (!evaled_) {
      ::new (&value_) Value(func_());
      func_ = Func();
      evaled_.store(1, std::memory_order_release);
    }
  }

public:
  Delay() = default;
  Delay(const Delay& rhs) {
    std::unique_lock<std::mutex>  guard(rhs.lock_);
    int evaled = rhs.evaled_.load(std::memory_order_acquire);
    if (!evaled) {
      func_ = rhs.func_;
    } else {
      ::new (&value_) Value(*reinterpret_cast<Value*>(std::addressof(rhs.value_)));
    }
    evaled_.store(evaled);
  }

  Delay(Value const& value) : evaled_(true) {
    ::new (&value_) Value(value);
  }

  Delay(Value&& value) : evaled_(true) {
    ::new (&value_) Value(std::move(value));
  }

  template <typename Action,
            typename = typename std::enable_if<isFuncConv<Action>::value>::type>
  Delay(Action&& A) : func_(std::forward<Action>(A)), evaled_(false) {
  }

  ~Delay() {
    if (evaled_) {
      reinterpret_cast<Value*>(std::addressof(this->value_))->~Value();
    }
  }

  Value const& get() const {
    int evaled = evaled_.load(std::memory_order_acquire);
    if (!evaled) {
      setValue();
    }
    return *reinterpret_cast<Value*>(std::addressof(this->value_));
  }

  operator Value const&() const {
    return get();
  }

  bool isForced() {
    return evaled_;
  }
};

template <typename Value>
Value const& force(Delay<Value> const& delay) {
  return delay;
}

template <typename Value>
Value const& force(Delay<Value>&& delay) {
  return std::move(delay);
}

template <typename F, typename... Args>
auto delay(F&& f, Args&&... args) -> Delay<decltype(f(args...))> {
  using Value = decltype(f(args...));
  return Delay<Value>(
      [ args..., f_ = std::forward<F>(f) ]() { return f_(args...); });
}

#endif
