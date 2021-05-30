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

template <typename Result>
class Thunk {
    struct Promise : public Holder<Result>::Promise {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::Holder<Result>>(this);
            return Thunk(std::move(holder));
        }
    };

  public:
    using promise_type = Promise;

  public:
    Thunk() : result_() {}

    Thunk(Thunk const& source) : result_(source.result_) {}

    Thunk(Thunk&& source) : result_(std::move(source.result_)) {}

    Thunk(std::shared_ptr<co_fun::Holder<Result>>&& r) : result_(r) {}

    explicit Thunk(Result const& r)
        : result_(std::make_shared<co_fun::Holder<Result>>(r)) {}

    explicit Thunk(Result&& r)
        : result_(std::make_shared<co_fun::Holder<Result>>(std::move(r))) {}

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

    Result const& get() const& {
        if (!evaluated()) {
            result_->resume();
        }
        return result_->get_value();
    }

    operator Result const &() const { return get(); }

  private:
    std::shared_ptr<co_fun::Holder<Result>> result_;
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

template <typename Value>
Value const& evaluate(Thunk<Value> const& thunk) {
    return thunk;
}

template <typename Value>
Value evaluate(Thunk<Value>&& thunk) {
    return std::move(thunk);
}

template <typename F, typename... Args>
auto thunk(F f, Args... args) -> Thunk<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

} // namespace co_fun

#endif
