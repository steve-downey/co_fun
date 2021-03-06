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

Thunk<std::string> co_func_str() { co_return std::string("a string"); }

std::string stringTest(const char* str) { return str; }
} // namespace

TEST(Co_FunThunkTest, Breathing) {
    Thunk<int> thunk;
    EXPECT_EQ(false, thunk.evaluated());
    EXPECT_EQ(true, thunk.isEmpty());

    Thunk<int> thunk2(std::move(thunk));
    EXPECT_EQ(false, thunk2.evaluated());
    EXPECT_EQ(true, thunk2.isEmpty());
    Thunk<int> thunk3(3);
    EXPECT_EQ(true, thunk3.evaluated());
    EXPECT_EQ(false, thunk3.isEmpty());

    int j{thunk3};

    EXPECT_EQ(3, j);

    Thunk<int> D2 = co_func();
    EXPECT_EQ(0, func_called);
    EXPECT_EQ(false, D2.evaluated());
    EXPECT_EQ(false, D2.isEmpty());
    int i = D2;
    EXPECT_EQ(i, 5);
    EXPECT_EQ(1, func_called);

    int i2 = D2;
    EXPECT_EQ(i2, 5);
    EXPECT_EQ(1, func_called);
}

TEST(Co_FunThunkTest, Rvalue) {
    Thunk<std::string> s1 = co_func_str();
    EXPECT_EQ(false, s1.evaluated());
    EXPECT_EQ(false, s1.isEmpty());
    EXPECT_EQ(std::string("a string"), evaluate(s1));

    EXPECT_EQ(std::string("a string"), evaluate(co_func_str()));
}

TEST(Co_FunThunkTest, Assignment) {
    Thunk<int> t3(3);
    Thunk<int> t1;
    t1 = t3;
}

TEST(Co_FunThunkTest, Move) {
    std::string        str;
    Thunk<std::string> d1(str);
    Thunk<std::string> d2("test");
    Thunk<std::string> d3 = thunk(stringTest, "this is a test");
    //    Thunk<std::string> d4([]() { return stringTest("another test");
    //    });

    EXPECT_TRUE(d1.evaluated());
    EXPECT_TRUE(d2.evaluated());
    EXPECT_FALSE(d3.evaluated());
    // EXPECT_FALSE(d4.evaluated());

    EXPECT_EQ(std::string("this is a test"), evaluate(d3));
    //    EXPECT_EQ(std::string("another test"), force(d4));
}

TEST(Co_FunThunkTest, Sharing) {
    Thunk<int> thunk = co_func();
    Thunk      t2    = thunk;
    Thunk      t3    = t2;

    int k = thunk;
    EXPECT_EQ(k, 5);
    EXPECT_TRUE(t3.evaluated());
}

struct watch_destruction {
    static int destructor_counter;

    ~watch_destruction() { ++destructor_counter; }
};

int watch_destruction::destructor_counter = 0;

Thunk<int> test_destruction(watch_destruction w) {
    co_return w.destructor_counter;
}

TEST(Co_FunThunkTest, Leak) {
    watch_destruction::destructor_counter = 0;
    {
        auto t = test_destruction(watch_destruction{});
        EXPECT_EQ(watch_destruction::destructor_counter, 1);
        int i = t;
        EXPECT_EQ(i, 1);
        EXPECT_EQ(watch_destruction::destructor_counter, 2);
    }
    EXPECT_EQ(watch_destruction::destructor_counter, 2);
    watch_destruction::destructor_counter = 0;
    {
        auto t2 = thunk(test_destruction, watch_destruction{});
        EXPECT_EQ(watch_destruction::destructor_counter, 1);
        // No evaluation of thunk - potential leak
    }
    EXPECT_EQ(watch_destruction::destructor_counter, 2);
}

TEST(Co_FunThunkTest, TransformTest) {
    auto t = thunk([]() { return 5; });
    auto i = transform(std::move(t), [](auto j) { return j * 2; });
    EXPECT_TRUE(t.isEmpty());
    EXPECT_EQ(i, 10);
    EXPECT_TRUE(t.isEmpty());

    auto i2 = transform(Thunk<int>{6}, [](auto j) { return j * 2; });
    EXPECT_EQ(i2, 12);

    auto t3 = thunk([]() { return 5; });
    auto i3 = transform(t3, [](auto j) { return j * 2; });
    EXPECT_FALSE(t3.evaluated());
    EXPECT_EQ(i3, 10);
    EXPECT_TRUE(t3.evaluated());
}

TEST(Co_FunThunkTest, JoinTest) {
    Thunk<Thunk<int>> l = thunk([]() { return Thunk{5}; });
    Thunk<int>       j = join(l);
    EXPECT_EQ(j, 5);

    Thunk<int>       l2 = thunk([](auto i, auto j) { return i + j; }, 3, 5);
    Thunk<Thunk<int>> l3 = [](auto l) -> Thunk<Thunk<int>> {
        co_return l;
    }(std::move(l2));
    Thunk<int> l4 = join(l3);
    EXPECT_EQ(false, l4.evaluated());
    EXPECT_EQ(8, l4);
}

TEST(Co_FunThunkTest, BindTest) {
    Thunk<int>    l = thunk([]() { return 5; });
    Thunk<double> b =
        bind(l, [](int i) -> Thunk<double> { co_return 1.0 * i; });
    EXPECT_EQ(false, b.evaluated());
    EXPECT_EQ(5.0, b);
    EXPECT_EQ(true, b.evaluated());
}

TEST(Co_FunThunkTest, BindTestRvalue) {
    Thunk<double> b = bind(thunk([]() { return 5; }),
                           [](int i) -> Thunk<double> { co_return 1.0 * i; });
    EXPECT_EQ(false, b.evaluated());
    EXPECT_EQ(5.0, b);
    EXPECT_EQ(true, b.evaluated());
}

TEST(Co_FunThunkTest, Bind2Test) {
    Thunk<int>    l = thunk([]() { return 5; });
    Thunk<double> b =
        bind2(l, [](int i) -> Thunk<double> { co_return 1.0 * i; });
    EXPECT_EQ(false, b.evaluated());
    EXPECT_EQ(5.0, b);
    EXPECT_EQ(true, b.evaluated());
}

TEST(Co_FunThunkTest, Bind2TestRvalue) {
    Thunk<double> b = bind2(thunk([]() { return 5; }),
                            [](int i) -> Thunk<double> { co_return 1.0 * i; });
    EXPECT_EQ(false, b.evaluated());
    EXPECT_EQ(5.0, b);
    EXPECT_EQ(true, b.evaluated());
}


} // namespace testing
