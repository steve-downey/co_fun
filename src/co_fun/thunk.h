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
    struct promise_type : public holder_promise_type<R> {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::holder<R>>(this);
            this->r_p   = holder.get();
            return Thunk(this, std::move(holder));
        }
    };

  public:
    using promise_type = promise_type;

  public:
    Thunk() : promise_(nullptr), result_() {}

    Thunk(Thunk const& source)
        : promise_(source.promise_), result_(source.result_) {
    }

    Thunk(Thunk&& source)
        : promise_(std::exchange(source.promise_, nullptr)),
          result_(std::move(source.result_)) {
    }

    Thunk(promise_type* p, std::shared_ptr<co_fun::holder<R>>&& r)
        : promise_(p), result_(r) {
    }

    explicit Thunk(R const& r)
        : promise_(nullptr), result_(std::make_shared<co_fun::holder<R>>(r)) {
    }

    explicit Thunk(R&& r)
        : promise_(nullptr),
          result_(std::make_shared<co_fun::holder<R>>(std::move(r))) {
    }

    ~Thunk() = default;

    Thunk& operator=(const Thunk& rhs) {
        promise_ = rhs.promise_;
        result_  = rhs.result_;
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
    bool evaluated() const { return result_ && !result_->is_empty(); }

    bool isEmpty() const {
        return (promise_ == nullptr) &&
               (result_ == nullptr || result_->is_empty());
    }

    R const& get() const {
        if (!evaluated()) {
            result_->promise()->handle().resume();
        }
        return result_->get_value();
    }

    operator R const &() const { return get(); }

  private:
    promise_type*                      promise_;
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
