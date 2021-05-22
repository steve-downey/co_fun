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
#include <functional>

#include <co_fun/holder.h>

namespace co_fun {

template <typename R>
class Thunk {
    struct promise_type {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::holder<R>>();
            r_p         = holder.get();
            return Thunk(this, std::move(holder));
        }

        void return_value(R v) {
            r_p->set_value(std::move(v));
            return;
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
    Thunk() : p_(nullptr), r_() {}

    Thunk(Thunk const& source) : p_(source.p_), r_(source.r_) {}

    Thunk(Thunk&& source)
        : p_(std::exchange(source.p_, nullptr)), r_(std::move(source.r_)) {}

    Thunk(promise_type* p, std::shared_ptr<co_fun::holder<R>>&& r)
        : p_(p), r_(r) {}

    explicit Thunk(R const& r)
        : p_(nullptr), r_(std::make_shared<co_fun::holder<R>>(r)) {}

    explicit Thunk(R&& r)
        : p_(nullptr), r_(std::make_shared<co_fun::holder<R>>(std::move(r))) {}

    ~Thunk() = default;

    Thunk& operator=(const Thunk& rhs) {
        p_ = rhs.p_;
        r_ = rhs.r_;
        return *this;
    }

    bool operator==(const Thunk& rhs) const {
        if (r_ == rhs.r_)
            return true;
        return false;
    }
    bool operator!=(const Thunk& rhs) const {
        if (r_ == rhs.r_)
            return false;
        return true;
    }
    bool evaluated() const { return r_ && !r_->is_empty(); }
    bool isEmpty() const {
        return (p_ == nullptr) && (r_ == nullptr || r_->is_empty());
    }
    R const& get() const {
        if (!evaluated()) {
            p_->handle().resume();
        }
        return r_->get_value();
    }

    operator R const &() const { return get(); }

  private:
    promise_type*                      p_;
    std::shared_ptr<co_fun::holder<R>> r_;
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

template <typename Value>
Value const& evaluate(Thunk<Value> const& thunk) {
    return thunk;
}

template <typename Value>
Value const& evaluate(Thunk<Value>&& thunk) {
    return std::move(thunk);
}

template <typename F, typename... Args>
auto thunk(F f, Args... args) -> Thunk<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

} // namespace co_fun

#endif
