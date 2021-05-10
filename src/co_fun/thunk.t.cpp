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

int func2(int i) {
    func2_called++;
    return i;
}

int func3(int i, int j) {
    func3_called++;
    return i + j;
}
} // namespace
} // namespace testing

std::string stringTest(const char* str) { return str; }

TEST(Co_FunThunkTest, Breathing) {
    Thunk<int> thunk(1);

    int j{thunk};

    EXPECT_EQ(1, j);

    // Thunk<int> D2(func);
    // EXPECT_EQ(0, func_called);
}
