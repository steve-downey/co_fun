#include <co_fun/continuation.h>

#include <gtest/gtest.h>
#include <type_traits>

using namespace co_fun;

TEST(Co_FunContinuationTest, TestGTest) { ASSERT_EQ(1, 1); }


TEST(Co_FunContinuationTest, Breathing) {
    Cont<int, int> c;
    Cont<int, int> c2;
    c2 = c;

    int five = 5;
    Cont<int, int> c3(five);

    int j = c3([](auto i){return i*2;});
}
