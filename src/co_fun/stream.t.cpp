#include <co_fun/stream.h>

#include <gtest/gtest.h>

using namespace co_fun;

namespace testing {
namespace {

class Int {
    int i_;

  public:
    Int() : i_(0){};
    explicit Int(int i) : i_(i){};
    explicit operator int() { return i_; }

    Int operator+(int j) const { return Int(i_ + j); }
};

bool operator==(Int lhs, Int rhs) {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

bool operator>(Int lhs, Int rhs) {
    return static_cast<int>(lhs) > static_cast<int>(rhs);
}

Int operator+(Int lhs, Int rhs) {
    return Int(static_cast<int>(lhs) + static_cast<int>(rhs));
}

class Double {
    double d_;

  public:
    Double() : d_(0){};
    explicit Double(double d) : d_(d){};
    explicit operator double() { return d_; }
};

bool operator==(Double lhs, Double rhs) {
    return static_cast<double>(lhs) == static_cast<double>(rhs);
};

} // namespace

TEST(Co_FunStreamTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(Co_FunStreamTest, breathingTest) {
    ConsCell<int> cc2(1);

    ConsStream<int> cs1;
    ConsStream<int> cs2 = make<ConsStream>(1);
    ConsStream<int> cs3(1);

    EXPECT_TRUE(cs1.isEmpty());
    EXPECT_FALSE(cs2.isEmpty());
    EXPECT_EQ(1, cs2.head());
    EXPECT_FALSE(cs3.isEmpty());
    EXPECT_EQ(1, cs3.head());

    int j = -1;
    EXPECT_NO_THROW({ j = cc2.head(); });
    EXPECT_EQ(1, j);

    ConsStream<int> str2 = cc2.tail();

    EXPECT_TRUE(str2.isEmpty());

    int             k    = cs2.head();
    ConsStream<int> str3 = cs2.tail();

    EXPECT_EQ(1, k);
    EXPECT_TRUE(str3.isEmpty());
}

TEST(Co_FunStreamTest, consTest) {
    ConsStream<int> cs(0);
    ConsStream<int> cs1    = cons(1, cs);
    ConsStream<int> stream = cons(2, cs1);

    EXPECT_EQ(0, cs.head());
    EXPECT_EQ(1, cs1.head());

    EXPECT_EQ(2, stream.head());
    EXPECT_EQ(1, stream.tail().head());
    EXPECT_EQ(0, stream.tail().tail().head());
    EXPECT_TRUE(stream.tail().tail().tail().isEmpty());
}

TEST(Co_FunStreamTest, takeStream) {
    ConsStream<int> from0to4 = rangeFrom(0, 4);
    ConsStream<int> stream   = take(from0to4, 3);

    EXPECT_EQ(0, stream.head());
    EXPECT_EQ(1, stream.tail().head());
    EXPECT_EQ(2, stream.tail().tail().head());
    EXPECT_TRUE(stream.tail().tail().tail().isEmpty());

    ConsStream<int>::iterator i = stream.begin();
    ConsStream<int>::iterator e = stream.end();

    EXPECT_EQ(0, *i);
    i++;
    EXPECT_EQ(1, *i);
    i++;
    EXPECT_EQ(2, *i);
    i++;
    EXPECT_EQ(i, e);

    int k = 0;
    for (auto const& a : stream) {
        EXPECT_EQ(k, a);
        ++k;
    }
}

TEST(Co_FunStreamTest, takeInfStream) {
    ConsStream<int> inf    = iota(0);
    ConsStream<int> stream = take(inf, 3);

    EXPECT_EQ(0, stream.head());
    EXPECT_EQ(1, stream.tail().head());
    EXPECT_EQ(2, stream.tail().tail().head());
    EXPECT_TRUE(stream.tail().tail().tail().isEmpty());

    ConsStream<int>::iterator i = stream.begin();
    ConsStream<int>::iterator e = stream.end();

    EXPECT_EQ(0, *i);
    i++;
    EXPECT_EQ(1, *i);
    i++;
    EXPECT_EQ(2, *i);
    i++;
    EXPECT_EQ(i, e);

    int k = 0;
    for (auto const& a : stream) {
        EXPECT_EQ(k, a);
        ++k;
    }
}

TEST(Co_FunStreamTest, appendStream) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s1  = take(inf, 3);
    ConsStream<int> s2  = take(iota(1), 3);
    ConsStream<int> c   = append(s1, s2);

    std::vector<int> v{0, 1, 2, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
}

TEST(Co_FunStreamTest, fmapStream) {
    ConsStream<int> inf = iota(0);

    ConsStream<int> square = fmap(inf, [](int i) { return i * i; });
    ConsStream<int> front  = take(square, 5);

    std::vector<int> v{0, 1, 4, 9, 16, 25};
    int              k = 0;
    for (auto const& a : front) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(5, k);

    ConsStream<double> square2 =
        fmap(inf, [](int i) -> double { return i * i; });
    ConsStream<double> front2 = take(square2, 5);

    std::vector<int> v2{0, 1, 4, 9, 16, 25};
    int              k2 = 0;
    for (auto const& a : front2) {
        EXPECT_EQ(v2[k2], a);
        ++k2;
    }
    EXPECT_EQ(5, k2);
}

TEST(Co_FunStreamTest, fmapStream2) {
    ConsStream<int> inf = iota(0);

    ConsStream<Int> square = fmap(inf, [](int i) { return Int(i * i); });
    ConsStream<Int> front  = take(square, 5);

    std::vector<int> v{0, 1, 4, 9, 16, 25};
    int              k = 0;
    for (auto const& a : front) {
        EXPECT_EQ(Int(v[k]), a);
        ++k;
    }
    EXPECT_EQ(5, k);

    ConsStream<Double> square2 =
        fmap(inf, [](int i) -> Double { return Double(i * i); });
    ConsStream<Double> front2 = take(square2, 5);

    std::vector<int> v2{0, 1, 4, 9, 16, 25};
    int              k2 = 0;
    for (auto const& a : front2) {
        EXPECT_EQ(Double(v2[k2]), a);
        ++k2;
    }
    EXPECT_EQ(5, k2);
}

TEST(Co_FunStreamTest, countEvaluated) {
    ConsStream<int> inf = iota(0);
    EXPECT_EQ(0, inf.countEvaluated());

    ConsStream<int> take3 = take(inf, 3);
    EXPECT_EQ(0, take3.countEvaluated());

    int k = 0;
    for (auto const& a : take3) {
        EXPECT_EQ(k, a);
        ++k;
    }
    EXPECT_EQ(3, k);
    EXPECT_EQ(3, take3.countEvaluated());
    EXPECT_EQ(3, inf.countEvaluated());

    ConsStream<int> iota3 = iota(3);
    ConsStream<int> inf2  = append(take3, iota3);
    EXPECT_EQ(0, inf2.head());
    EXPECT_EQ(1, inf2.countEvaluated());
    EXPECT_EQ(0, iota3.countEvaluated());
}

TEST(Co_FunStreamTest, concatStreamList) {
    ConsStream<int>             inf = iota(0);
    ConsStream<int>             s1  = take(inf, 3);
    // ConsStream<ConsStream<int>> s2(take(iota(1), 3));
    // ConsStream<ConsStream<int>> stream = cons(s1, s2);

    // ConsStream<int> c = concat(stream);

    // std::vector<int> v{0, 1, 2, 1, 2, 3};
    // int              k = 0;
    // for (auto const& a : c) {
    //     EXPECT_EQ(v[k], a);
    //     ++k;
    // }
    // EXPECT_EQ(6, k);
}

TEST(Co_FunStreamTest, concatStreamList2) {
    ConsStream<int>             inf = iota(0);
    ConsStream<ConsStream<int>> s2 =
        fmap(inf, [](int i) { return rangeFrom(0, i); });
    ConsStream<ConsStream<int>> s3 = take(s2, 3);

    ConsStream<int> c = concat(s3);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(3, inf.countEvaluated());
    EXPECT_EQ(3, s2.countEvaluated());
    EXPECT_EQ(3, s3.countEvaluated());
    EXPECT_EQ(6, c.countEvaluated());
}

TEST(Co_FunStreamTest, concatInfStreamList) {
    ConsStream<int>             inf = iota(0);
    ConsStream<ConsStream<int>> s2 =
        fmap(inf, [](int i) { return rangeFrom(0, i); });

    ConsStream<int> c = concat(s2);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : take(c, 10)) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(10, k);

    EXPECT_EQ(5, inf.countEvaluated());
    EXPECT_EQ(5, s2.countEvaluated());
    EXPECT_EQ(10, c.countEvaluated());
}

TEST(Co_FunStreamTest, concatInfStreamList2) {
    ConsStream<Int>             inf = iota(Int(0));
    ConsStream<ConsStream<Int>> s2 =
        fmap(inf, [](Int i) { return rangeFrom(Int(0), i); });

    ConsStream<Int> c = concat(s2);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : take(c, 10)) {
        EXPECT_EQ(Int(v[k]), a);
        ++k;
    }
    EXPECT_EQ(10, k);

    EXPECT_EQ(5, inf.countEvaluated());
    EXPECT_EQ(5, s2.countEvaluated());
    EXPECT_EQ(10, c.countEvaluated());
}

TEST(Co_FunStreamTest, joinStreamList) {
    ConsStream<int>             inf = iota(0);
    ConsStream<ConsStream<int>> s2 =
        fmap(inf, [](int i) { return rangeFrom(0, i); });
    ConsStream<int> s3 = join(s2);

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(4, s2.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, joinStreamList2) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3 =
        join(fmap(inf, [](int i) { return rangeFrom(0, i); }));

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, bindStreamList) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3  = bind(inf, [](int i) { return rangeFrom(0, i);});

    EXPECT_EQ(0, s3.head());

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, bindStreamList2) {
    ConsStream<int> inf = iota(0);
    ConsStream<Int> s3 =
        bind(inf, [](int i) { return rangeFrom(Int(0), Int(i)); });

    EXPECT_EQ(Int(), s3.head());

    ConsStream<Int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(Int(v[k]), a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, bind2StreamList) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3  = bind2(inf, [](int i) { return rangeFrom(0, i);
});

    EXPECT_EQ(0, s3.head());

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, bind2StreamList2) {
    ConsStream<int> inf = iota(0);
    ConsStream<Int> s3 =
        bind2(inf, [](int i) { return rangeFrom(Int(0), Int(i)); });

    EXPECT_EQ(Int(), s3.head());

    ConsStream<Int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(Int(v[k]), a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, thenStreamList) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3  = then(inf, []() { return rangeFrom(0, 2); });

    EXPECT_EQ(0, s3.head());

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(3, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, then2StreamList) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3  = then2(inf, []() { return rangeFrom(0, 2); });

    EXPECT_EQ(0, s3.head());

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(3, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, join2StreamList) {
    ConsStream<int>             inf = iota(0);
    ConsStream<ConsStream<int>> s2 =
        fmap(inf, [](int i) { return rangeFrom(0, i); });
    ConsStream<int> s3 = join2(s2);

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(4, s2.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, join2StreamList2) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s3 =
        join2(fmap(inf, [](int i) { return rangeFrom(0, i); }));

    ConsStream<int> c = take(s3, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(4, inf.countEvaluated());
    EXPECT_EQ(6, s3.countEvaluated());
}

TEST(Co_FunStreamTest, guard) {
    EXPECT_FALSE(guard(true).isEmpty());
    EXPECT_TRUE(guard(false).isEmpty());
}

namespace {
ConsStream<std::tuple<int, int, int>> triples() {
    return bind(iota(1), [](int z) {
        return bind(rangeFrom(1, z), [z](int x) {
            return bind(rangeFrom(x, z), [x, z](int y) {
                return then(guard(x * x + y * y == z * z), [x, y, z]() {
                    return make<ConsStream>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}

ConsStream<std::tuple<int, int, int>> triples2() {
    return bind2(iota(1), [](int z) {
        return bind2(rangeFrom(1, z), [z](int x) {
            return bind2(rangeFrom(x, z), [x, z](int y) {
                return then2(guard(x * x + y * y == z * z), [x, y, z]() {
                    return make<ConsStream>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}
}

TEST(Co_FunStreamTest, pythag) {
    auto trip = triples();
    EXPECT_EQ(std::make_tuple(3, 4, 5), trip.head());

    EXPECT_EQ(1, trip.countEvaluated());

    auto tenTrips = take(trip, 10);

    std::tuple<int, int, int> tenth;
    for (auto const& a : tenTrips) {
        tenth = a;
    }
    EXPECT_EQ(std::make_tuple(20, 21, 29), tenth);
    EXPECT_EQ(10, trip.countEvaluated());
}

TEST(Co_FunStreamTest, pythag2) {
    auto trip = triples2();
    EXPECT_EQ(std::make_tuple(3, 4, 5), trip.head());

    EXPECT_EQ(1, trip.countEvaluated());

    auto tenTrips = take(trip, 10);

    std::tuple<int, int, int> tenth;
    for (auto const& a : tenTrips) {
        tenth = a;
    }
    EXPECT_EQ(std::make_tuple(20, 21, 29), tenth);
    EXPECT_EQ(10, trip.countEvaluated());
}

TEST(Co_FunStreamTest, strictFuncs) {
    auto from35 = rangeFrom(3, 5);
    EXPECT_EQ(5, last(from35));
    auto init35 = init(from35);
    EXPECT_EQ(4, last(init35));
    EXPECT_EQ(3u, length(from35));
    EXPECT_EQ(2u, length(init35));
}

TEST(Co_FunStreamTest, filter) {
    auto inf     = iota(1);
    auto even    = filter([](int i) { return 0 == (i % 2); }, inf);
    auto first10 = take(even, 10);
    EXPECT_EQ(2, even.head());
    EXPECT_EQ(20, last(first10));
}

TEST(Co_FunStreamTest, drop) {
    auto inf    = iota(3);
    auto drop10 = drop(inf, 10);
    EXPECT_EQ(13, drop10.head());
}

class MoveableFuncional {
    int j_;

  public:
    MoveableFuncional(int j) : j_(j) {}

    MoveableFuncional(MoveableFuncional const& f) = delete;

    MoveableFuncional(MoveableFuncional&& f) : j_(std::move(f.j_)) {}

    int operator()(int i) const { return i + j_; }
};

TEST(Co_FunStreamTest, dot) {
    auto f     = [](int i) { return i + 1; };
    auto g     = [](int j) { return 2 * j; };
    auto comp  = dot(f, g);
    auto comp2 = dot(g, f);

    EXPECT_EQ(5, comp(2));
    EXPECT_EQ(8, comp2(3));

    auto itos = [](int i) {
        std::ostringstream s;
        s << i;
        return s.str();
    };
    auto comp3 = dot(itos, f);
    EXPECT_EQ("6", comp3(5));

    auto two   = [](int i, int j) { return i + 2 * j; };
    auto comp4 = dot(g, two);

    EXPECT_EQ(10, comp4(1, 2));

    MoveableFuncional m(3);

    auto comp5 = dot(g, std::move(m));
    EXPECT_EQ(12, comp5(3));
}

TEST(Co_FunStreamTest, concatMapTest) {
    auto list = [](int i) { return rangeFrom(0, i); };

    ConsStream<int> mapped = concatMap(list, rangeFrom(0, 4));

    ConsStream<int> c = take(mapped, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(6, mapped.countEvaluated());
}

TEST(Co_FunStreamTest, concatMapTest2) {
    auto list = [](int i) { return rangeFrom(0, i); };

    ConsStream<int> mapped = concatMap(list, iota(0));

    ConsStream<int> c = take(mapped, 6);

    std::vector<int> v{0, 0, 1, 0, 1, 2, 0, 1, 2, 3};
    int              k = 0;
    for (auto const& a : c) {
        EXPECT_EQ(v[k], a);
        ++k;
    }
    EXPECT_EQ(6, k);

    EXPECT_EQ(6, mapped.countEvaluated());
}

TEST(Co_FunStreamTest, foldrTest) {
    auto inf    = iota(1);
    auto even   = filter([](int i) { return 0 == (i % 2); }, inf);
    auto first5 = take(even, 5);

    auto func = [](int i, int j) { return i + j; };

    auto c = foldr(func, 0, first5);
    EXPECT_EQ(30, c);

    EXPECT_EQ(5, first5.countEvaluated());
}

TEST(Co_FunStreamTest, foldrTest2) {
    auto inf    = iota(1);
    auto even   = filter([](int i) { return 0 == (i % 2); }, inf);
    auto first5 = take(even, 5);

    auto func = [](int i, Int j) { return Int(i) + j; };

    auto c = foldr(func, Int(0), first5);
    EXPECT_EQ(Int(30), c);

    EXPECT_EQ(5, first5.countEvaluated());
}

TEST(Co_FunStreamTest, appTest) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s1  = take(inf, 3);

    auto                    add1Lambda = [](int i) { return i + 1; };
    std::function<int(int)> add1       = add1Lambda;
    auto                    streamFunc = make<ConsStream>(add1);

    auto result = app(streamFunc, s1);

    EXPECT_EQ(1, result.head());
}

TEST(Co_FunStreamTest, app2Test) {
    ConsStream<int> inf = iota(0);
    ConsStream<int> s1  = take(inf, 3);

    auto                    add1Lambda = [](int i) { return i + 1; };
    std::function<int(int)> add1       = add1Lambda;
    auto                    streamFunc = make<ConsStream>(add1);

    auto result = app2(streamFunc, s1);

    EXPECT_EQ(1, result.head());
}

} // namespace testing

namespace {
class NoDefault {
  public:
    NoDefault(int);
    NoDefault() = delete;
};

class NoMove {
  public:
    NoMove(NoMove&&) = delete;
    NoMove(NoMove const&){};
    NoMove& operator=(NoMove const&) { return *this; };
};

} // namespace testing

namespace co_fun {
template class ConsStreamIterator<int>;
template class ConsStream<std::string>;
template class ConsStream<NoDefault>;
}
