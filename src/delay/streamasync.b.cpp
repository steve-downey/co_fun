#include <benchmark/benchmark.h>

#include <delay/delayasync.h>
#include <delay/streamasync.h>

#include <sstream>
#include <string>

static void BM_ConcatAsync(benchmark::State& state) {
    auto x = state.range(0);
    int  l = 0;
    while (state.KeepRunning()) {
        ConsStreamAsync<int>                  inf = iota(0);
        ConsStreamAsync<int>                  s1  = take(inf, x);
        ConsStreamAsync<ConsStreamAsync<int>> s2(take(iota(1), x));
        ConsStreamAsync<ConsStreamAsync<int>> streamAsync = cons(s1, s2);
        ConsStreamAsync<int>                  c = concat(streamAsync);
        l                                       = last(c);
    }
    std::stringstream ss;
    ss << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_ConcatAsync)->Arg(8)->Arg(64)->Arg(512);

static void BM_ConcatMapAsync(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                           = 0;
        auto                 list   = [](int i) { return rangeFrom(0, i); };
        ConsStreamAsync<int> mapped = concatMap(list, rangeFrom(1l, x));
        // Eat the streamAsync so not recursively destroying 100K sharedptrs
        while (!mapped.tail().isEmpty()) {
            mapped = mapped.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_ConcatMapAsync)->Arg(8)->Arg(64)->Arg(512);

static void BM_JoinAsync(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                                         = 0;
        ConsStreamAsync<int>                  inf = iota(0);
        ConsStreamAsync<ConsStreamAsync<int>> s2 =
            fmap(inf, [](int i) { return rangeFrom(0, i); });
        ConsStreamAsync<int> s3 = join(s2);
        ConsStreamAsync<int> c  = take(s3, x);
        // Eat the streamAsync so not recursively destroying 100K sharedptrs
        while (!c.tail().isEmpty()) {
            c = c.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_JoinAsync)->Arg(8)->Arg(64)->Arg(512);

static void BM_Join2Async(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        l                                         = 0;
        ConsStreamAsync<int>                  inf = iota(0);
        ConsStreamAsync<ConsStreamAsync<int>> s2 =
            fmap(inf, [](int i) { return rangeFrom(0, i); });
        ConsStreamAsync<int> s3 = join2(s2);
        ConsStreamAsync<int> c  = take(s3, x);
        // Eat the streamAsync so not recursively destroying 100K sharedptrs
        while (!c.tail().isEmpty()) {
            c = c.tail();
            l++;
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Join2Async)->Arg(8)->Arg(64)->Arg(512);

static void BM_BindAsync(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        {
            state.ResumeTiming();
            l = 0;
            ConsStreamAsync<int> c =
                take(bind2(iota(0), [](int i) { return rangeFrom(0, i); }), x);
            l = last(c);
            state.PauseTiming();
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_BindAsync)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 10)->Arg(8 << 10);

static void BM_Bind2Async(benchmark::State& state) {
    auto   x = state.range(0);
    size_t l = 0;
    while (state.KeepRunning()) {
        {
            state.ResumeTiming();
            l = 0;
            ConsStreamAsync<int> c =
                take(bind2(iota(0), [](int i) { return rangeFrom(0, i); }), x);
            l = last(c);
            state.PauseTiming();
        }
    }
    std::stringstream ss;
    ss << x << ',' << l;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Bind2Async)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(1 << 10)
    ->Arg(8 << 10);

namespace {
ConsStreamAsync<std::tuple<int, int, int>> triples() {
    return bind(iota(1), [](int z) {
        return bind(rangeFrom(1, z), [z](int x) {
            return bind(rangeFrom(x, z), [x, z](int y) {
                return then(guardAsync(x * x + y * y == z * z), [x, y, z]() {
                    return make<ConsStreamAsync>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}
} // namespace

static void BM_Triple1Async(benchmark::State& state) {
    int x = 0;
    int y = 0;
    int z = 0;
    while (state.KeepRunning())
        std::tie(x, y, z) = last(take(triples(), 10));

    std::stringstream ss;
    ss << x << ',' << y << ',' << z;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Triple1Async);
BENCHMARK(BM_Triple1Async)->UseRealTime();

namespace {
ConsStreamAsync<std::tuple<int, int, int>> triples2() {
    return bind2(iota(1), [](int z) {
        return bind2(rangeFrom(1, z), [z](int x) {
            return bind2(rangeFrom(x, z), [x, z](int y) {
                return then2(guardAsync(x * x + y * y == z * z), [x, y, z]() {
                    return make<ConsStreamAsync>(std::make_tuple(x, y, z));
                });
            });
        });
    });
}
} // namespace
static void BM_Triple2Async(benchmark::State& state) {
    int x = 0;
    int y = 0;
    int z = 0;
    while (state.KeepRunning())
        std::tie(x, y, z) = last(take(triples2(), 10));

    std::stringstream ss;
    ss << x << ',' << y << ',' << z;
    state.SetLabel(ss.str());
}
BENCHMARK(BM_Triple2Async);
BENCHMARK(BM_Triple2Async)->UseRealTime();
