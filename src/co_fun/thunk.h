// thunk.h                                                            -*-C++-*-
#ifndef INCLUDED_THUNK
#define INCLUDED_THUNK

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

#include <coroutine>
#include <memory>

#include <co_fun/holder.h>

namespace co_fun {

template <typename R>
class Thunk {
    struct promise_type {
        co_fun::holder<R> r_;

        auto get_return_object() { return Thunk(this); }

        auto return_value(R v) {
            r_.set_value(std::move(v));
            return std::suspend_always();
        }

        void unhandled_exception() { throw; }

        std::suspend_always initial_suspend() noexcept { return {}; }

        std::suspend_never final_suspend() noexcept { return {}; }

        auto handle() {
            return std::coroutine_handle<promise_type>::from_promise(*this);
        }
    };

  public:
    using promise_type = promise_type;

  public:
    Thunk(Thunk&& source) : p_(std::exchange(source.p_, nullptr)) {}

    explicit Thunk(promise_type* p) : p_(p) {}

    ~Thunk() {
        if (p_)
            p_->handle().destroy();
    }

    //    Thunk(R r) {}

    operator R() const {
        if (!p_->r_) {
            p_->handle().resume();
        }
        return *(p_->r_);
    }

  private:
    promise_type* p_;
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
