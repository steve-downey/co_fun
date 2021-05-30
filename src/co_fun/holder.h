// holder.h                                                           -*-C++-*-
#ifndef INCLUDED_CO_FUN_HOLDER
#define INCLUDED_CO_FUN_HOLDER

#include <atomic>
#include <exception>
#include <utility>
#include <cassert>
#include <coroutine>

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

namespace co_fun {

template <typename T>
struct value {
    T   value;
    T&& get_value() { return static_cast<T&&>(value); }
};

template <>
struct value<void> {
    void get_value() {}
};

template <typename R>
struct Holder {
    struct Promise {
        void return_value(R v) {
            holder_->set_value(std::move(v));
            return;
        }

        void unhandled_exception() { throw; }

        std::suspend_always initial_suspend() noexcept { return {}; }

        std::suspend_never final_suspend() noexcept { return {}; }

        auto handle() {
            return std::coroutine_handle<Promise>::from_promise(*this);
        }

        void setHolder(Holder<R>* holder) { holder_ = holder; }

        Holder<R>* holder_;
    };

    enum class result_status { empty, value, error };

    std::atomic<result_status> status{result_status::empty};

    union result_holder {
        result_holder(){};
        ~result_holder(){};

        value<R>           wrapper;
        std::exception_ptr error;
    } result_;

    Promise* promise_;

    template <typename... Args>
    void set_value(Args&&... args) {
        new (std::addressof(result_.wrapper))
            value<R>{std::forward<Args>(args)...};

        status.store(result_status::value, std::memory_order_release);
    }

    void unhandled_exception() noexcept {
        new (std::addressof(result_.error))
            std::exception_ptr(std::current_exception());

        status.store(result_status::error, std::memory_order_release);
    }

    bool unevaluated() const noexcept {
        return status.load(std::memory_order_relaxed) == result_status::empty;
    }

    R&& get_value() {
        switch (status.load(std::memory_order_acquire)) {
        case result_status::empty: {
            assert(false);
            std::terminate();
            break;
        }
        case result_status::value: {
            return result_.wrapper.get_value();
        }
        case result_status::error: {
            std::rethrow_exception(std::exchange(result_.error, {}));
            break;
        }
        }
        assert(false);
        std::terminate();
    }

    void resume() { return promise_->handle().resume(); }

    Promise* promise() { return promise_; }

    Holder() : promise_(nullptr) {}

    Holder(Promise* p) : promise_(p) { p->setHolder(this); }

    Holder(Holder&& source)
        : promise_(std::exchange(source.promise_, nullptr)) {}

    Holder(R t) : promise_(nullptr) {
        new (std::addressof(result_.wrapper)) value<R>{t};

        status.store(result_status::value, std::memory_order_release);
    }

    ~Holder() {
        switch (status.load(std::memory_order_relaxed)) {
        case result_status::empty: {
            if (promise_)
                promise_->handle().destroy();
            break;
        }
        case result_status::value: {
            result_.wrapper.~value();
            break;
        }
        case result_status::error: {
            if (result_.error)
                std::rethrow_exception(result_.error);
            result_.error.~exception_ptr();
        } break;
        }
    }

};

} // namespace co_fun

#endif
