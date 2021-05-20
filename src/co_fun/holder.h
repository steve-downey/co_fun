// holder.h                                                           -*-C++-*-
#ifndef INCLUDED_CO_FUN_HOLDER
#define INCLUDED_CO_FUN_HOLDER

#include <atomic>
#include <exception>
#include <utility>
#include <cassert>

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

template <typename T>
struct holder {
    enum class result_status { empty, value, error };

    std::atomic<result_status> status{result_status::empty};

    union result_holder {
        result_holder(){};
        ~result_holder(){};

        value<T>           wrapper;
        std::exception_ptr error;
    } result;

    template <typename... Args>
    void set_value(Args&&... args) {
        new (std::addressof(result.wrapper))
            value<T>{std::forward<Args>(args)...};

        status.store(result_status::value, std::memory_order_release);
    }

    void unhandled_exception() noexcept {
        new (std::addressof(result.error))
            std::exception_ptr(std::current_exception());

        status.store(result_status::error, std::memory_order_release);
    }

    bool is_empty() const noexcept {
        return status.load(std::memory_order_relaxed) == result_status::empty;
    }

    T&& get_value() {
        switch (status.load(std::memory_order_acquire)) {
        case result_status::empty: {
            assert(false);
            std::terminate();
            break;
        }
        case result_status::value: {
            return result.wrapper.get_value();
        }
        case result_status::error: {
            std::rethrow_exception(std::exchange(result.error, {}));
            break;
        }
        }
        assert(false);
        std::terminate();
    }

    holder() = default;

    holder(T t) {
        new (std::addressof(result.wrapper)) value<T>{t};

        status.store(result_status::value, std::memory_order_release);
    }

    ~holder() {
        switch (status.load(std::memory_order_relaxed)) {
        case result_status::empty: {
            break;
        }
        case result_status::value: {
            result.wrapper.~value();
            break;
        }
        case result_status::error: {
            if (result.error)
                std::rethrow_exception(result.error);
            result.error.~exception_ptr();
        } break;
        }
    }
};
// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

} // namespace co_fun

#endif
