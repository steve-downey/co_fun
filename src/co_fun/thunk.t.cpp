#include <co_fun/thunk.h>

#include <gtest/gtest.h>

using namespace co_fun;

TEST(Co_FunThunkTest, TestGTest) { ASSERT_EQ(1, 1); }

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

Thunk<int> co_func() {
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

TEST(Co_FunThunkTest, Breathing) {
    // Thunk<int> thunk(1);

    // int j{thunk};

    // EXPECT_EQ(1, j);

    Thunk<int> D2 = co_func();
    // EXPECT_EQ(0, func_called);
    // int i = D2;
    // EXPECT_EQ(i, 5);
    // EXPECT_EQ(1, func_called);

    // int i2 = D2;
    // EXPECT_EQ(i2, 5);
    // EXPECT_EQ(1, func_called);

    //    Thunk<int> D3 = D2;
}

} // namespace testing
