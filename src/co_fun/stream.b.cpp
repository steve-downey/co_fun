#include <benchmark/benchmark.h>

#include <co_fun/thunk.h>
#include <co_fun/stream.h>

#include <sstream>
#include <string>

using namespace co_fun;

int Factorial(uint32_t n) { return (n == 1) ? 1 : n * Factorial(n - 1); }

static void BM_Concat(benchmark::State& state) {
    auto x = state.range(0);
    int  l = 0;
    while (state.KeepRunning()) {
        ConsStream<int>             inf = iota(0);
        ConsStream<int>             s1  = take(inf, x);
        ConsStream<ConsStream<int>> s2(take(iota(1), x));
        ConsStream<ConsStream<int>> stream = cons(s1, s2);
        ConsStream<int>             c      = concat(stream);
        l                                  = last(c);
    }
    std::stringstream ss;
    ss << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Concat)->Arg(8)->Arg(64)->Arg(512);

static void BM_ConcatMap(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                      = 0;
        auto            list   = [](int i) { return rangeFrom(0, i); };
        ConsStream<int> mapped = concatMap(list, rangeFrom(1l, x));
        // Eat the stream so not recursively destroying 100K sharedptrs
        while (!mapped.tail().isEmpty()) {
            mapped = mapped.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_ConcatMap)->Arg(8)->Arg(64)->Arg(512);

static void BM_Join(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                               = 0;
        ConsStream<int>             inf = iota(0);
        ConsStream<ConsStream<int>> s2 =
            fmap(inf, [](int i) { return rangeFrom(0, i); });
        ConsStream<int> s3 = join(s2);
        ConsStream<int> c  = take(s3, x);
        // Eat the stream so not recursively destroying 100K sharedptrs
        while (!c.tail().isEmpty()) {
            c = c.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Join)->Arg(8)->Arg(64)->Arg(512);

static void BM_Join2(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                               = 0;
        ConsStream<int>             inf = iota(0);
        ConsStream<ConsStream<int>> s2 =
            fmap(inf, [](int i) { return rangeFrom(0, i); });
        ConsStream<int> s3 = join2(s2);
        ConsStream<int> c  = take(s3, x);
        // Eat the stream so not recursively destroying 100K sharedptrs
        while (!c.tail().isEmpty()) {
            c = c.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Join2)->Arg(8)->Arg(64)->Arg(512);

static void BM_Bind(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        {
            state.ResumeTiming();
            l = 0;
            ConsStream<int> c =
                take(bind(iota(0), [](int i) { return rangeFrom(0, i); }), x);
            l = last(c);
            state.PauseTiming();
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Bind)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 10)->Arg(8 << 10);

static void BM_Bind2(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        {
            state.ResumeTiming();
            l = 0;
            ConsStream<int> c =
                take(bind2(iota(0), [](int i) { return rangeFrom(0, i); }), x);
            l = last(c);
            state.PauseTiming();
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Bind2)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 10)->Arg(8 << 10);

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

static void BM_Triple1(benchmark::State& state) {
    int x = 0;
    int y = 0;
    int z = 0;
    while (state.KeepRunning())
        std::tie(x, y, z) = last(take(triples(), 10));

    std::stringstream ss;
    ss << x << ',' << y << ',' << z;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Triple1);
BENCHMARK(BM_Triple1)->UseRealTime();

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

static void BM_Triple2(benchmark::State& state) {
    int x = 0;
    int y = 0;
    int z = 0;
    while (state.KeepRunning())
        std::tie(x, y, z) = last(take(triples2(), 10));

    std::stringstream ss;
    ss << x << ',' << y << ',' << z;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Triple2);
BENCHMARK(BM_Triple2)->UseRealTime();

BENCHMARK_MAIN();
