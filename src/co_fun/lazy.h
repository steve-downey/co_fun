// lazy.h                                                             -*-C++-*-
#ifndef INCLUDED_LAZY
#define INCLUDED_LAZY

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

#include <cassert>
#include <coroutine>
#include <memory>
#include <functional>

#include <co_fun/holder.h>

namespace co_fun {

template <typename Result>
class Lazy {
    struct Promise : public Holder<Result>::Promise {
        auto get_return_object() {
            auto holder = std::make_unique<co_fun::Holder<Result>>(this);
            return Lazy(std::move(holder));
        }
    };

  public:
    using promise_type = Promise;

  public:
    Lazy() : result_() {}

    Lazy(Lazy&& source) : result_(std::move(source.result_)) {}

    explicit Lazy(std::unique_ptr<co_fun::Holder<Result>>&& result)
        : result_(std::move(result)) {}

    explicit Lazy(Result result)
        : result_(std::make_unique<co_fun::Holder<Result>>(result)) {}

    ~Lazy() = default;

    bool evaluated() const { return result_ && !result_->unevaluated(); }

    bool isEmpty() const {
        bool empty = false;
        if (!result_) {
            empty = true;
        } else if (result_->isNil()) {
            empty = true;
        }
        return empty;
    }

    Result&& get() const {
        if (!evaluated()) {
            result_->resume();
        }
        return result_->get_value();
    }

    operator Result&&() const { return get(); }

  private:
    std::unique_ptr<co_fun::Holder<Result>> result_;
};

template <typename Value>
Value const& evaluate(Lazy<Value> const& lazy) {
    return lazy;
}

template <typename Value>
Value&& evaluate(Lazy<Value>&& lazy) {
    return std::move(lazy);
}

template <typename F, typename... Args>
auto lazy(F f, Args... args) -> Lazy<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

template <typename Result, typename F>
auto transform(Lazy<Result> l, F f) -> Lazy<std::invoke_result_t<F, Result>> {
    co_return f(evaluate(l));
}

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
