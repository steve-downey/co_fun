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
    struct promise_type {
        auto get_return_object() {
            auto holder = std::make_unique<co_fun::holder<R>>();
            r_p         = holder.get();
            return Lazy(this, std::move(holder));
        }

        auto return_value(R v) {
            r_p->set_value(std::move(v));
            return std::suspend_always();
        }

        void unhandled_exception() { throw; }

        std::suspend_always initial_suspend() noexcept { return {}; }

        std::suspend_never final_suspend() noexcept { return {}; }

        auto handle() {
            return std::coroutine_handle<promise_type>::from_promise(*this);
        }

        co_fun::holder<R>* r_p;
    };

  public:
    using promise_type = promise_type;

  public:
    Lazy() : p_(nullptr), r_() {}

    Lazy(Lazy&& source)
        : p_(std::exchange(source.p_, nullptr)), r_(std::move(source.r_)) {}

    explicit Lazy(promise_type* p, std::unique_ptr<co_fun::holder<R>>&& r)
        : p_(p), r_(std::move(r)) {}

    explicit Lazy(R r)
        : p_(nullptr), r_(std::make_unique<co_fun::holder<R>>(r)) {}

    ~Lazy() = default;

    bool evaluated() const { return r_ && !r_->is_empty(); }

    R&& get() const {
        if (!evaluated()) {
            p_->handle().resume();
        }
        return r_->get_value();
    }

    operator R&&() const { return get(); }

  private:
    promise_type*                      p_;
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
