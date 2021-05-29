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

template <typename R>
class Lazy {
    struct promise_type : public holder_promise_type<R> {
        auto get_return_object() {
            auto holder = std::make_unique<co_fun::holder<R>>(this);
            this->r_p   = holder.get();
            return Lazy(std::move(holder));
        }
    };

  public:
    using promise_type = promise_type;

  public:
    Lazy() : r_() {}

    Lazy(Lazy&& source)
        : r_(std::move(source.r_)) {}

    explicit Lazy(std::unique_ptr<co_fun::holder<R>>&& r)
        : r_(std::move(r)) {}

    explicit Lazy(R r)
        : r_(std::make_unique<co_fun::holder<R>>(r)) {}

    ~Lazy() = default;

    bool evaluated() const { return r_ && !r_->unevaluated(); }

    R&& get() const {
        if (!evaluated()) {
            r_->promise()->handle().resume();
        }
        return r_->get_value();
    }

    operator R&&() const { return get(); }

  private:
    std::unique_ptr<co_fun::holder<R>> r_;
};

template <typename Value>
Value const& evaluate(Lazy<Value> const& lazy) {
    return lazy;
}

template <typename Value>
Value const& evaluate(Lazy<Value>&& lazy) {
    return std::move(lazy);
}

template <typename F, typename... Args>
auto lazy(F f, Args... args) -> Lazy<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
