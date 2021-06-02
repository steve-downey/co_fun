#include <co_fun/holder.h>

#include <gtest/gtest.h>

using namespace co_fun;

TEST(Co_FunHolderTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(Co_FunHolderTest, ValueBreathing) {
    Value<int> vi;
    int        i = vi.get_value();
    (void)(i); // unused warning

    Value<void> vv;
    vv.get_value();
}

TEST(Co_FunHolderTest, HolderBreathing) {
    Holder<int> hi;
    hi.set_value(42);
    int i = hi.get_value();
    EXPECT_EQ(42, i);

    Value<void> vv;
    vv.get_value();
}
