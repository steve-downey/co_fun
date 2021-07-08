// continuation.h                                                     -*-C++-*-
#ifndef INCLUDED_CONTINUATION
#define INCLUDED_CONTINUATION

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

#include <co_fun/holder.h>
#include <memory>

namespace co_fun {

template <typename K, typename R, typename A>
concept Kont = requires(K k, R r, A a) {
    r = k(a);
};

template <typename A, typename R>
class Cont {
  public:
    using Continuation = auto(A) -> R;
    using CTR          = auto(Continuation) -> R;

  private:
    struct Promise : public Holder<A>::Promise {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::Holder<CTR>>(this);
            return Cont(std::move(holder));
        }
    };

  public:
    using promise_type = Promise;

  private:
    bool evaluated() const { return a_ && !a_->unevaluated(); }

    A const& get() const& {
        if (!evaluated()) {
            a_->resume();
        }
        return a_->get_value();
    }

  public:
    auto operator()(Continuation k) -> R { return (k)(get()); }

    Cont() : a_() {}

    explicit Cont(A const& a) : a_(std::make_shared<co_fun::Holder<A>>(a)){};

    explicit Cont(A&& a)
        : a_(std::make_shared<co_fun::Holder<A>>(std::move(a))) {}

    Cont(std::shared_ptr<co_fun::Holder<A>>&& a) : a_(a) {}

  private:
    std::shared_ptr<co_fun::Holder<A>> a_;
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
