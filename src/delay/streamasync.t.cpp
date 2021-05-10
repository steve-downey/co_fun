#include <delay/streamasync.h>

#include "gtest/gtest.h"
using ::testing::Test;

namespace testing {
namespace {

class Int
{
  int i_;
public:
  Int() : i_(0){};
  explicit  Int(int i) : i_(i){};
  explicit operator int(){return i_;}

  Int operator+(int j) const {return Int(i_ + j);}
};

bool operator==(Int lhs, Int rhs){
  return static_cast<int>(lhs) == static_cast<int>(rhs);
}

bool operator>(Int lhs, Int rhs){
  return static_cast<int>(lhs) > static_cast<int>(rhs);
}

Int operator+(Int lhs, Int rhs){
  return Int(static_cast<int>(lhs) + static_cast<int>(rhs));
}


class Double
{
  double d_;
public:
  Double() : d_(0){};
  explicit Double(double d) : d_(d){};
  explicit operator double(){return d_;}
};

bool operator==(Double lhs, Double rhs){
  return static_cast<double>(lhs) == static_cast<double>(rhs);
};


}

class StreamAsyncTest : public Test {
protected:
  StreamAsyncTest() {
  }
  ~StreamAsyncTest() {
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

public:
};

TEST_F(StreamAsyncTest, breathingTest) {
    ConsCell<int> cc2(1);

    ConsStreamAsync<int> cs1;
    ConsStreamAsync<int> cs2 = make<ConsStreamAsync>(1);
    ConsStreamAsync<int> cs3(1);

    EXPECT_TRUE(cs1.isEmpty());
    EXPECT_FALSE(cs2.isEmpty());
    EXPECT_FALSE(cs3.isEmpty());

    int j = -1;
    EXPECT_NO_THROW({
            j = cc2.head();
        });
    EXPECT_EQ(1, j);

    ConsStreamAsync<int> str2 = cc2.tail();

    EXPECT_TRUE(str2.isEmpty());

    int k = cs2.head();
    ConsStreamAsync<int> str3 = cs2.tail();

    EXPECT_EQ(1, k);
    EXPECT_TRUE(str3.isEmpty());

}

TEST_F(StreamAsyncTest, consTest) {
  ConsStreamAsync<int> cs(0);
  ConsStreamAsync<int> cs1 = cons(1, cs);
  ConsStreamAsync<int> streamAsync = cons(2, cs1);

  EXPECT_EQ(2, streamAsync.head());
  EXPECT_EQ(1, streamAsync.tail().head());
  EXPECT_EQ(0, streamAsync.tail().tail().head());
  EXPECT_TRUE(streamAsync.tail().tail().tail().isEmpty());
}

TEST_F(StreamAsyncTest, finiteStreamAsync) {
  ConsStreamAsync<int> streamAsync = rangeFrom(0, 2);
  EXPECT_EQ(0, streamAsync.head());
  EXPECT_EQ(1, streamAsync.tail().head());
  EXPECT_EQ(2, streamAsync.tail().tail().head());
  EXPECT_TRUE(streamAsync.tail().tail().tail().isEmpty());

  ConsStreamAsync<int>::iterator i = streamAsync.begin();
  ConsStreamAsync<int>::iterator e = streamAsync.end();

  EXPECT_EQ(0, *i);
  i++;
  EXPECT_EQ(1, *i);
  i++;
  EXPECT_EQ(2, *i);
  i++;
  EXPECT_EQ(i, e);

  int k = 0;
  for(auto const& a : streamAsync) {
    EXPECT_EQ(k, a);
    ++k;
  }
}

TEST_F(StreamAsyncTest, takeStreamAsync) {
  ConsStreamAsync<int> from0to4 = rangeFrom(0, 4);
  ConsStreamAsync<int> streamAsync = take(from0to4, 3);

  EXPECT_EQ(0, streamAsync.head());
  EXPECT_EQ(1, streamAsync.tail().head());
  EXPECT_EQ(2, streamAsync.tail().tail().head());
  EXPECT_TRUE(streamAsync.tail().tail().tail().isEmpty());

  ConsStreamAsync<int>::iterator i = streamAsync.begin();
  ConsStreamAsync<int>::iterator e = streamAsync.end();

  EXPECT_EQ(0, *i);
  i++;
  EXPECT_EQ(1, *i);
  i++;
  EXPECT_EQ(2, *i);
  i++;
  EXPECT_EQ(i, e);

  int k = 0;
  for(auto const& a : streamAsync) {
    EXPECT_EQ(k, a);
    ++k;
  }
}

TEST_F(StreamAsyncTest, takeInfStreamAsync) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> streamAsync = take(inf, 3);

  EXPECT_EQ(0, streamAsync.head());
  EXPECT_EQ(1, streamAsync.tail().head());
  EXPECT_EQ(2, streamAsync.tail().tail().head());
  EXPECT_TRUE(streamAsync.tail().tail().tail().isEmpty());

  ConsStreamAsync<int>::iterator i = streamAsync.begin();
  ConsStreamAsync<int>::iterator e = streamAsync.end();

  EXPECT_EQ(0, *i);
  i++;
  EXPECT_EQ(1, *i);
  i++;
  EXPECT_EQ(2, *i);
  i++;
  EXPECT_EQ(i, e);

  int k = 0;
  for(auto const& a : streamAsync) {
    EXPECT_EQ(k, a);
    ++k;
  }
}

TEST_F(StreamAsyncTest, appendStreamAsync) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s1 = take(inf, 3);
  ConsStreamAsync<int> s2 = take(iota(1), 3);
  ConsStreamAsync<int> c = append(s1, s2);

  std::vector<int> v{0,1,2,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
}

TEST_F(StreamAsyncTest, fmapStreamAsync) {
  ConsStreamAsync<int> inf = iota(0);

  ConsStreamAsync<int> square = fmap(inf, [](int i){return i*i;});
  ConsStreamAsync<int> front = take(square, 5);

  std::vector<int> v{0,1,4,9,16,25};
  int k = 0;
  for(auto const& a : front) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(5, k);

  ConsStreamAsync<double> square2 = fmap(inf, [](int i) -> double {return i*i;});
  ConsStreamAsync<double> front2 = take(square2, 5);

  std::vector<int> v2{0,1,4,9,16,25};
  int k2 = 0;
  for(auto const& a : front2) {
    EXPECT_EQ(v2[k2], a);
    ++k2;
  }
  EXPECT_EQ(5, k2);
}

TEST_F(StreamAsyncTest, fmapStreamAsync2) {
  ConsStreamAsync<int> inf = iota(0);

  ConsStreamAsync<Int> square = fmap(inf, [](int i){return Int(i*i);});
  ConsStreamAsync<Int> front = take(square, 5);

  std::vector<int> v{0,1,4,9,16,25};
  int k = 0;
  for(auto const& a : front) {
    EXPECT_EQ(Int(v[k]), a);
    ++k;
  }
  EXPECT_EQ(5, k);

  ConsStreamAsync<Double> square2 = fmap(inf, [](int i) -> Double {return Double(i*i);});
  ConsStreamAsync<Double> front2 = take(square2, 5);

  std::vector<int> v2{0,1,4,9,16,25};
  int k2 = 0;
  for(auto const& a : front2) {
    EXPECT_EQ(Double(v2[k2]), a);
    ++k2;
  }
  EXPECT_EQ(5, k2);
}

TEST_F(StreamAsyncTest, countForced) {
  ConsStreamAsync<int> inf = iota(0);
  EXPECT_EQ(0, inf.countForced());

  ConsStreamAsync<int> take3 = take(inf, 3);
  EXPECT_EQ(0, take3.countForced());

  int k = 0;
  for(auto const& a : take3) {
    EXPECT_EQ(k, a);
    ++k;
  }
  EXPECT_EQ(3, k);
  EXPECT_EQ(3, take3.countForced());
  EXPECT_EQ(3, inf.countForced());

  ConsStreamAsync<int> iota3 = iota(3);
  ConsStreamAsync<int> inf2 = append(take3, iota3);
  EXPECT_EQ(0, inf2.head());
  EXPECT_EQ(1, inf2.countForced());
  EXPECT_EQ(0, iota3.countForced());
}

TEST_F(StreamAsyncTest, concatStreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s1 = take(inf, 3);
  ConsStreamAsync<ConsStreamAsync<int>> s2(take(iota(1), 3));
  ConsStreamAsync<ConsStreamAsync<int>> streamAsync = cons(s1, s2);

  ConsStreamAsync<int> c = concat(streamAsync);

  std::vector<int> v{0,1,2,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);
}

TEST_F(StreamAsyncTest, concatStreamAsyncList2) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<ConsStreamAsync<int>> s2 = fmap(inf, [](int i){return rangeFrom(0, i);});
  ConsStreamAsync<ConsStreamAsync<int>> s3 = take(s2, 3);

  ConsStreamAsync<int> c = concat(s3);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(3, inf.countForced());
  EXPECT_EQ(3, s2.countForced());
  EXPECT_EQ(3, s3.countForced());
  EXPECT_EQ(6, c.countForced());
}

TEST_F(StreamAsyncTest, concatInfStreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<ConsStreamAsync<int>> s2 = fmap(inf, [](int i){return rangeFrom(0, i);});

  ConsStreamAsync<int> c = concat(s2);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : take(c, 10)) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(10, k);

  EXPECT_EQ(5, inf.countForced());
  EXPECT_EQ(5, s2.countForced());
  EXPECT_EQ(10, c.countForced());
}

TEST_F(StreamAsyncTest, concatInfStreamAsyncList2) {
  ConsStreamAsync<Int> inf = iota(Int(0));
  ConsStreamAsync<ConsStreamAsync<Int>> s2 = fmap(inf, [](Int i){return rangeFrom(Int(0), i);});

  ConsStreamAsync<Int> c = concat(s2);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : take(c, 10)) {
    EXPECT_EQ(Int(v[k]), a);
    ++k;
  }
  EXPECT_EQ(10, k);

  EXPECT_EQ(5, inf.countForced());
  EXPECT_EQ(5, s2.countForced());
  EXPECT_EQ(10, c.countForced());
}

TEST_F(StreamAsyncTest, joinStreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<ConsStreamAsync<int>> s2 = fmap(inf, [](int i){return rangeFrom(0, i);});
  ConsStreamAsync<int> s3 = join(s2);

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(4, s2.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, joinStreamAsyncList2) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = join(fmap(inf, [](int i){return rangeFrom(0, i);}));

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, bindStreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = bind(inf,
                            [](int i){return rangeFrom(0, i);});

  EXPECT_EQ(0, s3.head());

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, bindStreamAsyncList2) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<Int> s3 = bind(inf,
                            [](int i){return rangeFrom(Int(0), Int(i));});

  EXPECT_EQ(Int(), s3.head());

  ConsStreamAsync<Int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(Int(v[k]), a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, bind2StreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = bind2(inf,
                             [](int i){return rangeFrom(0, i);});

  EXPECT_EQ(0, s3.head());

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, bind2StreamAsyncList2) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<Int> s3 = bind2(inf,
                             [](int i){return rangeFrom(Int(0), Int(i));});

  EXPECT_EQ(Int(), s3.head());

  ConsStreamAsync<Int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(Int(v[k]), a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, thenStreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = then(inf,
                            [](){return rangeFrom(0, 2);});

  EXPECT_EQ(0, s3.head());

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,1,2,0,1,2,0,1,2,0};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(3, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, then2StreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = then2(inf,
                            [](){return rangeFrom(0, 2);});

  EXPECT_EQ(0, s3.head());

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,1,2,0,1,2,0,1,2,0};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(3, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, join2StreamAsyncList) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<ConsStreamAsync<int>> s2 = fmap(inf, [](int i){return rangeFrom(0, i);});
  ConsStreamAsync<int> s3 = join2(s2);

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(4, s2.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, join2StreamAsyncList2) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s3 = join2(fmap(inf, [](int i){return rangeFrom(0, i);}));

  ConsStreamAsync<int> c = take(s3, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(4, inf.countForced());
  EXPECT_EQ(6, s3.countForced());

}

TEST_F(StreamAsyncTest, guard) {
  EXPECT_FALSE(guardAsync(true).isEmpty());
  EXPECT_TRUE(guardAsync(false).isEmpty());
}


namespace {

ConsStreamAsync<std::tuple<int, int, int>> triples() {
  return bind(iota(1), [](int z) {
      return bind(rangeFrom(1, z), [z](int x) {
          return bind(rangeFrom(x, z), [x, z](int y) {
              return then(guardAsync(x*x + y*y == z*z), [x, y, z]() {
                  return make<ConsStreamAsync>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}

ConsStreamAsync<std::tuple<int, int, int>> triples2() {
  return bind2(iota(1), [](int z) {
      return bind2(rangeFrom(1, z), [z](int x) {
          return bind2(rangeFrom(x, z), [x, z](int y) {
              return then2(guardAsync(x*x + y*y == z*z), [x, y, z]() {
                  return make<ConsStreamAsync>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}
}

TEST_F(StreamAsyncTest, pythag) {
  auto trip = triples();
  EXPECT_EQ(std::make_tuple(3,4,5), trip.head());

  EXPECT_EQ(1, trip.countForced());

  auto tenTrips = take(trip, 10);

  std::tuple<int,int,int> tenth;
  for(auto const& a : tenTrips) {
    tenth = a;
  }
  EXPECT_EQ(std::make_tuple(20,21,29), tenth);
  EXPECT_EQ(10, trip.countForced());

}

TEST_F(StreamAsyncTest, pythag2) {
  auto trip = triples2();
  EXPECT_EQ(std::make_tuple(3,4,5), trip.head());

  EXPECT_EQ(1, trip.countForced());

  auto tenTrips = take(trip, 10);

  std::tuple<int,int,int> tenth;
  for(auto const& a : tenTrips) {
    tenth = a;
  }
  EXPECT_EQ(std::make_tuple(20,21,29), tenth);
  EXPECT_EQ(10, trip.countForced());

}


TEST_F(StreamAsyncTest, strictFuncs) {
  auto from35 = rangeFrom(3,5);
  EXPECT_EQ(5, last(from35));
  auto init35 = init(from35);
  EXPECT_EQ(4, last(init35));
  EXPECT_EQ(3u, length(from35));
  EXPECT_EQ(2u, length(init35));
}

TEST_F(StreamAsyncTest, filter) {
  auto inf = iota(1);
  auto even = filter([](int i) {return 0==(i%2);}, inf);
  auto first10 = take(even, 10);
  EXPECT_EQ(2, even.head());
  EXPECT_EQ(20, last(first10));
}

TEST_F(StreamAsyncTest, drop) {
  auto inf = iota(3);
  auto drop10 = drop(inf, 10);
  EXPECT_EQ(13, drop10.head());
}


class MoveableFuncional {
  int j_;
public:
  MoveableFuncional(int j)
      : j_(j){}

  MoveableFuncional(MoveableFuncional const& f) = delete;

  MoveableFuncional(MoveableFuncional&& f)
      : j_(std::move(f.j_)) {}

  int operator()(int i) const {
    return i + j_;
  }
};

TEST_F(StreamAsyncTest, dot) {
  auto f = [](int i) {return i+1;};
  auto g = [](int j) {return 2*j;};
  auto comp = dot(f,g);
  auto comp2 = dot(g,f);

  EXPECT_EQ(5, comp(2));
  EXPECT_EQ(8, comp2(3));

  auto itos = [](int i) {std::ostringstream s; s << i; return s.str(); };
  auto comp3 = dot(itos, f);
  EXPECT_EQ("6", comp3(5));

  auto two = [](int i, int j) {return i + 2*j;};
  auto comp4 = dot(g, two);

  EXPECT_EQ(10, comp4(1,2));

  MoveableFuncional m(3);

  auto comp5 = dot(g, std::move(m));
  EXPECT_EQ(12, comp5(3));
}

TEST_F(StreamAsyncTest, concatMapTest) {
  auto list = [](int i){return rangeFrom(0, i);};

  ConsStreamAsync<int> mapped = concatMap(list, rangeFrom(0, 4));

  ConsStreamAsync<int> c = take(mapped, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(6, mapped.countForced());
}

TEST_F(StreamAsyncTest, concatMapTest2) {
  auto list = [](int i){return rangeFrom(0, i);};

  ConsStreamAsync<int> mapped = concatMap(list, iota(0));

  ConsStreamAsync<int> c = take(mapped, 6);

  std::vector<int> v{0,0,1,0,1,2,0,1,2,3};
  int k = 0;
  for(auto const& a : c) {
    EXPECT_EQ(v[k], a);
    ++k;
  }
  EXPECT_EQ(6, k);

  EXPECT_EQ(6, mapped.countForced());
}

TEST_F(StreamAsyncTest, foldrTest) {
  auto inf = iota(1);
  auto even = filter([](int i) {return 0==(i%2);}, inf);
  auto first5 = take(even, 5);

  auto func = [](int i, int j){return i+j;};

  auto c = foldr(func, 0, first5);
  EXPECT_EQ(30, c);

  EXPECT_EQ(5, first5.countForced());
}

TEST_F(StreamAsyncTest, foldrTest2) {
  auto inf = iota(1);
  auto even = filter([](int i) {return 0==(i%2);}, inf);
  auto first5 = take(even, 5);

  auto func = [](int i, Int j){return Int(i)+j;};

  auto c = foldr(func, Int(0), first5);
  EXPECT_EQ(Int(30), c);

  EXPECT_EQ(5, first5.countForced());
}


TEST_F(StreamAsyncTest, appTest) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s1 = take(inf, 3);

  auto add1Lambda = [](int i) {return i+1;};
  std::function<int(int)> add1 = add1Lambda;
  auto streamAsyncFunc = make<ConsStreamAsync>(add1);

  auto result = app(streamAsyncFunc,s1);

  EXPECT_EQ(1, result.head());

}

TEST_F(StreamAsyncTest, app2Test) {
  ConsStreamAsync<int> inf = iota(0);
  ConsStreamAsync<int> s1 = take(inf, 3);

  auto add1Lambda = [](int i) {return i+1;};
  std::function<int(int)> add1 = add1Lambda;
  auto streamAsyncFunc = make<ConsStreamAsync>(add1);

  auto result = app2(streamAsyncFunc,s1);

  EXPECT_EQ(1, result.head());

}


} // end namespace

namespace {
class NoDefault
{
public:
  NoDefault(int);
  NoDefault() = delete;
};

class NoMove
{
public:
  NoMove(NoMove&&) = delete;
  NoMove(NoMove const&){};
  NoMove& operator=(NoMove const&){return *this;};
};
}

template class ConsStreamAsyncIterator<int>;
template class ConsStreamAsync<std::string>;
template class ConsStreamAsync<NoDefault>;
