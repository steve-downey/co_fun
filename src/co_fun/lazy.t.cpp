#include <co_fun/lazy.h>

#include <gtest/gtest.h>
#include <type_traits>

using namespace co_fun;

TEST(Co_FunLazyTest, TestGTest) { ASSERT_EQ(1, 1); }

using ::testing::Test;

namespace testing {
namespace {
int func_called;
int func2_called;
int func3_called;
} // namespace

namespace {
int func() {
    func_called++;
    return 5;
}

Lazy<int> co_func() {
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

} // namespace

TEST(Co_FunLazyTest, Breathing) {
    Lazy<int> lazy;
    Lazy<int> lazy2(std::move(lazy));
    EXPECT_EQ(false, lazy2.evaluated());
    Lazy<int> lazy3(3);

    int j{lazy3};

    EXPECT_EQ(3, j);

    Lazy<int> D2 = co_func();
    EXPECT_EQ(0, func_called);
    int i = D2;
    EXPECT_EQ(i, 5);
    EXPECT_EQ(1, func_called);

    int i2 = D2;
    EXPECT_EQ(i2, 5);
    EXPECT_EQ(1, func_called);
}

// template <typename Value>
// Value  force(Lazy<Value> const& lazy) {
//     return lazy;
// }

// template <typename Value>
// Value const& force(Lazy<Value>&& lazy) {
//     return std::move(lazy);
// }

template <typename F, typename... Args>
auto lazy(F&& f, Args&&... args) -> Lazy<std::invoke_result_t<F, Args...>> {
    co_return std::invoke(f, args...);
}

TEST(Co_FunLazyTest, MoveTest) {
    std::string        str;
    Lazy<std::string> d1(str);
    Lazy<std::string> d2("test");
    Lazy<std::string> d3 = lazy(stringTest, "this is a test");
    //    Lazy<std::string> d4([]() { return stringTest("another test"); });

    EXPECT_TRUE(d1.evaluated());
    EXPECT_TRUE(d2.evaluated());
    EXPECT_FALSE(d3.evaluated());
    std::string s = d3;
    EXPECT_EQ(std::string("this is a test"), s);
    //    EXPECT_FALSE(d4.isForced());

    Lazy<std::string> d4 = lazy(stringTest, "this is another test");
    EXPECT_EQ(std::string("this is another test"), evaluate(d4));
    //    EXPECT_EQ(std::string("another test"), force(d4));
}

} // namespace testing
