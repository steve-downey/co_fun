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

#include <co_fun/thunk.h>
#include <memory>
#include <type_traits>

namespace co_fun {

template <typename A>
class Cont {

  private:
    struct Promise : public Holder<A>::Promise {
        auto get_return_object() {
            auto holder = std::make_shared<co_fun::Holder<A>>(this);
            return Cont(std::move(holder));
        }
    };

  public:
    using promise_type = Promise;

  public:
    template <typename Continuation>
    auto operator()(Continuation k) -> Thunk<std::invoke_result_t<Continuation, A>> {
        co_return(k)(a_);
    }

    Cont() : a_() {}

    explicit Cont(A const& a) : a_(a){};

    explicit Cont(A&& a) : a_(std::move(a)) {}

    explicit Cont(Thunk<A> const& a) : a_(a){};
    explicit Cont(Thunk<A>&& a) : a_(std::move(a)){};

    Cont(std::shared_ptr<co_fun::Holder<A>>&& a) : a_(a) {}

  private:
    Thunk<A> a_;
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
