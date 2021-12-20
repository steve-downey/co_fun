#include <std_lazy/std_lazy.h>

#include <gtest/gtest.h>

#include <iostream>
#include <functional>

TEST(StdLazyTest, TestGTest) { ASSERT_EQ(1, 1); }

using ::testing::Test;

std::lazy<void> f() {
    std::cout << "Hello, world!\n";
    co_return;
}

std::lazy<int> f(int x) { co_return x; }

std::lazy<int> sum(int x) {
    if (x <= 0) {
        std::cout << "negative sum\n";
        co_return co_await f(0);
    } else {
        std::cout << "pos branch\n";
        auto f_x = co_await f(x);
        std::cout << "f(x) = " << f_x << '\n';
        auto sum_x_1 = co_await sum(x - 1);
        std::cout << "sum(x-1) = " << sum_x_1 << '\n';
        co_return f_x + sum_x_1;
    }
}

std::lazy<int> sum1(int x) {
    if (x <= 0) {
        co_return co_await f(0);
    } else {
        auto f_x     = co_await f(x);
        auto sum_x_1 = co_await sum1(x - 1);
        co_return f_x + sum_x_1;
    }
}

std::lazy<int> sum2(int x) {
    if (x <= 0) {
        co_return f(0).sync_await();
    } else {
        auto f_x     = f(x).sync_await();
        auto sum_x_1 = sum2(x - 1).sync_await();
        co_return f_x + sum_x_1;
    }
}

template <typename F, typename... Args>
auto lazy_evaluate(F f, Args... args)
    -> std::lazy<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

namespace {
int func_called;
// int func2_called;
// int func3_called;
} // namespace

namespace {
int func() {
    func_called++;
    return 5;
}
/*
std::lazy<int> co_func() {
    func_called++;
    co_return 5;
}

int func2(int i) {
    func2_called++;
    return i;
}

int func3(int i, int j) {
    func3_called++;
    return i + j;
}
std::string stringTest(const char* str) { return str; }
*/
} // namespace

std::lazy<int> test(int k) {
    return lazy_evaluate([k]() {
        func_called++;
        return k;
    });
}

int  identity(int i) { return i; }
auto func_return() { return identity; }

auto evaluate(auto&& l) { return l.sync_await(); }

TEST(StdLazyTest, Basic) {
    {
        std::lazy<void> lz = f();
        lz.sync_await();
    }

    assert(f(42).sync_await() == 42);
    auto s_42 = sum(42);
    std::cout << "sync_await sum\n";
    assert(s_42.sync_await() == 42 * 43 / 2);

    auto s_42_2 = sum2(42);
    std::cout << "sync_await sum2\n";
    assert(s_42_2.sync_await() == 42 * 43 / 2);

    func_called = 0;
    auto l      = lazy_evaluate([]() { return func(); });
    assert(0 == func_called);
    int i = l.sync_await();
    assert(i == 5);
    assert(1 == func_called);

    auto l2 = test(9);
    assert(1 == func_called);
    int i2 = l2.sync_await();
    assert(i2 == 9);
    assert(2 == func_called);

    {
        auto l = lazy_evaluate(func_return);
        int  i = l.sync_await()(10);
        assert(i == 10);
    }

    {
        auto l = lazy_evaluate([]() { return 5; });
        int  i = l.sync_await();
        assert(i == 5);

        auto l2 = lazy_evaluate([]() { return []() { return 5; }; });
        int  i2 = evaluate(l2)();
        assert(i2 == 5);

        auto l3 = lazy_evaluate([](auto c) { return c; }, []() { return 5; });
        int  i3 = evaluate(l3)();
        assert(i3 == 5);

        auto l4 = lazy_evaluate([](auto c) { return c; },
                                [](int j) { return j + 5; });
        int  i4 = evaluate(l4)(1);
        assert(i4 == 6);

        auto lambda1 = [](int k) { return [k](int j) { return k + j; }; };
        auto l5      = lazy_evaluate(lambda1, 1);
        int  i5      = evaluate(l5)(6);
        assert(i5 == 7);
    }
}
