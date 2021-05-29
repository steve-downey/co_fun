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

#include <cassert>
#include <coroutine>
#include <memory>
#include <functional>

#include <co_fun/holder.h>
#include <utility>

namespace co_fun {

template <typename R>
class Thunk {
    struct promise_type : public holder_promise_type<R> {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::holder<R>>(this);
            this->r_p   = holder.get();
            return Thunk(std::move(holder));
        }
    };

  public:
    using promise_type = promise_type;

  public:
    Thunk() : result_() {}

    Thunk(Thunk const& source) : result_(source.result_) {}

    Thunk(Thunk&& source) : result_(std::move(source.result_)) {}

    Thunk(std::shared_ptr<co_fun::holder<R>>&& r) : result_(r) {}

    explicit Thunk(R const& r)
        : result_(std::make_shared<co_fun::holder<R>>(r)) {}

    explicit Thunk(R&& r)
        : result_(std::make_shared<co_fun::holder<R>>(std::move(r))) {}

    ~Thunk() = default;

    Thunk& operator=(const Thunk& rhs) {
        result_ = rhs.result_;
        return *this;
    }

    bool operator==(const Thunk& rhs) const {
        if (result_ == rhs.result_)
            return true;
        return false;
    }
    bool operator!=(const Thunk& rhs) const {
        if (result_ == rhs.result_)
            return false;
        return true;
    }
    bool evaluated() const { return result_ && !result_->unevaluated(); }

    bool isEmpty() const {
        bool empty = false;
        if (!result_) {
            empty = true;
        } else if (result_->unevaluated() && !result_->promise()) {
            empty = true;
        }
        return empty;
    }

    R const& get() const {
        if (!evaluated()) {
            result_->promise()->handle().resume();
        }
        return result_->get_value();
    }

    operator R const &() const { return get(); }

  private:
    std::shared_ptr<co_fun::holder<R>> result_;
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
