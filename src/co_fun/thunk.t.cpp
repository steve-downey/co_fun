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
    Thunk<int> thunk;
    Thunk<int> thunk2(std::move(thunk));
    EXPECT_EQ(false, thunk2.evaluated());
    Thunk<int> thunk3(3);

    int j{thunk3};

    EXPECT_EQ(3, j);

    Thunk<int> D2 = co_func();
    EXPECT_EQ(0, func_called);
    int i = D2;
    EXPECT_EQ(i, 5);
    EXPECT_EQ(1, func_called);

    int i2 = D2;
    EXPECT_EQ(i2, 5);
    EXPECT_EQ(1, func_called);
}

TEST(Co_FunThunkTest, Move) {
    std::string        str;
    Thunk<std::string> d1(str);
    Thunk<std::string> d2("test");
    Thunk<std::string> d3 = thunk(stringTest, "this is a test");
    //    Thunk<std::string> d4([]() { return stringTest("another test"); });

    EXPECT_TRUE(d1.evaluated());
    EXPECT_TRUE(d2.evaluated());
    EXPECT_FALSE(d3.evaluated());
    // EXPECT_FALSE(d4.evaluated());

    EXPECT_EQ(std::string("this is a test"), evaluate(d3));
 //    EXPECT_EQ(std::string("another test"), force(d4));
}

TEST(Co_FunThunkTest, Sharing) {
    Thunk<int> thunk = co_func();
    Thunk t2 = thunk;
    Thunk t3 = t2;

    int k = thunk;
    EXPECT_EQ(k, 5);
    EXPECT_TRUE(t3.evaluated());
}
} // namespace testing