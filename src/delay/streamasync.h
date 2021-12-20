// stream.h                                                           -*-C++-*-
#ifndef INCLUDED_STREAMASYNC
#define INCLUDED_STREAMASYNC

#include <delay/delay.h>
#include <optional>
#include <iterator>
#include <memory>
#include <tuple>

template <typename Value>
class ConsStreamAsync;

template <typename Value>
class ConsStreamAsyncIterator;

template <typename Value>
class ConsCell {
  Value head_;
  ConsStreamAsync<Value> tail_;

  friend class ConsStreamAsyncIterator<Value>;

public:
  ConsCell(Value const& v, ConsStreamAsync<Value> const& streamAsync)
      : head_(v), tail_(streamAsync) {
  }

  explicit ConsCell(Value const& v) : head_(v), tail_() {
  }

  Value const& head() const {
    return head_;
  }

  ConsStreamAsync<Value> const& tail() const {
    return tail_;
  }
};

template <typename Value>
class ConsStreamAsync {
  std::shared_ptr<Delay<ConsCell<Value>>> delayed_cell_;

  friend class ConsStreamAsyncIterator<Value>;

public:
  typedef Value value;

  ConsStreamAsync() = default;

  ConsStreamAsync(Value const& value)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(
                          [value]() { return ConsCell<Value>(value); })) {
  }

  ConsStreamAsync(Value && value)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(
                          [v = std::forward<Value>(value)]() { return ConsCell<Value>(v); })) {
  }

  template <typename Func,
            typename = typename std::enable_if<
              !std::is_convertible<Func, ConsStreamAsync>::value>::type>
  ConsStreamAsync(Func&& f)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(f)) {
  }

  bool isEmpty() const {
    return !delayed_cell_;
  }

  Value head() const {
    return force(*delayed_cell_).head();
  }

  ConsStreamAsync<Value> tail() const {
    return force(*delayed_cell_).tail();
  }

  typedef ConsStreamAsyncIterator<Value> iterator;

  iterator begin() {
    return iterator(delayed_cell_);
  };

  iterator end() {
    return iterator();
  }

  int countForced() {
    if (!delayed_cell_) {
      return 0;
    }

    auto cell = delayed_cell_;
    int forced = 0;
    while (cell && cell->isForced()) {
      ++forced;
      cell = cell->get().tail().delayed_cell_;
    }
    return forced;
  }
};

template <typename Value>
ConsStreamAsync<Value> make_streamAsync(Value v) {
  return ConsStreamAsync<Value>([v]() { return ConsCell<Value>(v); });
}


template <template<typename> typename Applicative, typename Value>
struct Make {
  Applicative<Value> operator()(Value const& v);
  Applicative<Value> operator()(Value && v);
};

template<typename Value>
struct Make<ConsStreamAsync, Value> {
  typedef typename std::decay<Value>::type V;
  ConsStreamAsync<V> operator()(Value const& v) {
    // return ConsStreamAsync<Value>([v]() { return ConsCell<Value>(v); });
    return ConsStreamAsync<V>(v);
  }
  ConsStreamAsync<V> operator()(V && v) {
    // return ConsStreamAsync<Value>([v]() { return ConsCell<Value>(v); });
    return ConsStreamAsync<V>(v);
  }
};

template <template<typename> typename Applicative, typename Value>
Applicative<typename std::decay<Value>::type> make(Value const& v){
  Make<Applicative, Value> m;
  return m(v);
}

template <template<typename> typename Applicative, typename Value>
Applicative<typename std::decay<Value>::type> make(Value && v){
  Make<Applicative, Value> m;
  return m(v);
}

template <typename Value>
class ConsStreamAsyncIterator : public std::iterator<std::forward_iterator_tag,
                                                std::remove_cv_t<Value>,
                                                std::ptrdiff_t,
                                                Value*,
                                                Value&> {
  std::shared_ptr<Delay<ConsCell<Value>>> delayed_cell_;

  explicit ConsStreamAsyncIterator(std::shared_ptr<Delay<ConsCell<Value>>> cell)
      : delayed_cell_(cell) {
  }

  friend class ConsStreamAsync<Value>;

public:
  ConsStreamAsyncIterator() = default; // Default construct gives end.

  void swap(ConsStreamAsyncIterator& other) noexcept {
    using std::swap;
    swap(delayed_cell_, other.delayed_cell_);
  }

  ConsStreamAsyncIterator& operator++() // Pre-increment
  {
    delayed_cell_ = force(*delayed_cell_).tail().delayed_cell_;
    return *this;
  }

  ConsStreamAsyncIterator operator++(int) // Post-increment
  {
    ConsStreamAsyncIterator tmp(*this);
    delayed_cell_ = force(*delayed_cell_).tail().delayed_cell_;
    return tmp;
  }

  // two-way comparison: v.begin() == v.cbegin() and vice versa
  template <class OtherType>
  bool operator==(const ConsStreamAsyncIterator<OtherType>& rhs) const {
    return delayed_cell_ == rhs.delayed_cell_;
  }

  template <class OtherType>
  bool operator!=(const ConsStreamAsyncIterator<OtherType>& rhs) const {
    return delayed_cell_ != rhs.delayed_cell_;
  }

  Value const& operator*() const {
    return force(*delayed_cell_).head_;
  }

  Value const* operator->() const {
    return &force(*delayed_cell_).head_;
  }
};

template <typename Value>
ConsStreamAsync<Value> cons(Value n, ConsStreamAsync<Value> const& streamAsync) {
  return ConsStreamAsync<Value>(
      [n, streamAsync]() { return ConsCell<Value>(n, streamAsync); });
}

template <typename Value>
Value last(ConsStreamAsync<Value> const& streamAsync) {
  ConsStreamAsync<Value> s = streamAsync;
  while (!s.tail().isEmpty()) {
    s = s.tail();
  }
  return s.head();
}

template<typename Value>
ConsStreamAsync<Value> init(ConsStreamAsync<Value> const& streamAsync) {
  if (streamAsync.tail().isEmpty()) {
    return ConsStreamAsync<Value>();
  }
  return cons(streamAsync.head(), init(streamAsync.tail()));
}

template <typename Value>
size_t lengthAcc(ConsStreamAsync<Value> const& streamAsync, size_t n) {
  if (streamAsync.isEmpty()) {
    return n;
  }
  return lengthAcc(streamAsync.tail(), n+1);
}

template <typename Value>
size_t length(ConsStreamAsync<Value> const& streamAsync) {
  return lengthAcc(streamAsync, 0);
}

template<typename Value, typename Predicate>
ConsStreamAsync<Value> filter(Predicate const& p, ConsStreamAsync<Value> streamAsync) {
  while (!streamAsync.isEmpty() && !p(streamAsync.head())) {
    streamAsync = streamAsync.tail();
  }

  if (streamAsync.isEmpty()) {
    return ConsStreamAsync<Value>();
  }

  return ConsStreamAsync<Value>(
      [p, streamAsync]() {
        return ConsCell<Value>(streamAsync.head(), filter(p, streamAsync.tail()));
      });
}

template <typename Value>
ConsStreamAsync<Value> rangeFrom(Value n, Value m) {
  if (n > m) {
    return ConsStreamAsync<Value>();
  }
  return ConsStreamAsync<Value>(
      [n, m]() { return ConsCell<Value>(n, rangeFrom(n + 1, m)); });
}

template <typename Value>
ConsStreamAsync<Value> iota(Value n = Value()) {
  return ConsStreamAsync<Value>([n]() { return ConsCell<Value>(n, iota(n + 1)); });
}

template <typename Value>
ConsStreamAsync<Value> take(ConsStreamAsync<Value> const& strm, int n) {
  if (n == 0 || strm.isEmpty()) {
    return ConsStreamAsync<Value>();
  }
  return ConsStreamAsync<Value>([strm, n]() {
      return ConsCell<Value>(strm.head(), take(strm.tail(), n - 1));
    });
}

template <typename Value>
ConsStreamAsync<Value> drop(ConsStreamAsync<Value> const& strm, int n) {
  if (strm.isEmpty()) {
    return ConsStreamAsync<Value>();
  }

  if (n == 0) {
    return strm;
  }

  return drop(strm.tail(), n-1);
}

template <typename Value>
ConsStreamAsync<Value> append(ConsStreamAsync<Value> const& first,
                         ConsStreamAsync<Value> const& second) {
  if (first.isEmpty()) {
    return second;
  }
  return ConsStreamAsync<Value>([first, second]() {
      return ConsCell<Value>(first.head(), append(first.tail(), second));
    });
}

template <typename Value>
ConsStreamAsync<Value> append(ConsStreamAsync<Value> const& first,
                         Delay<ConsStreamAsync<Value>> const& second) {
  if (first.isEmpty()) {
    return force(second);
  }
  return ConsStreamAsync<Value>([first, second]() {
      return ConsCell<Value>(first.head(), append(first.tail(), second));
    });
}

template <typename Value, typename Func>
auto fmap(ConsStreamAsync<Value> const& streamAsync, Func const& f)
  -> ConsStreamAsync<decltype(f(streamAsync.head()))> {
  using Mapped = decltype(f(streamAsync.head()));
  if (streamAsync.isEmpty()) {
    return ConsStreamAsync<Mapped>();
  }

  return ConsStreamAsync<Mapped>([streamAsync, f]() {
      return ConsCell<Mapped>(f(streamAsync.head()), fmap(streamAsync.tail(), f));
    });
}

/*
  foldr            :: (a -> b -> b) -> b -> [a] -> b
  foldr f z []     =  z
  foldr f z (x:xs) =  f x (foldr f z xs)
*/
template <typename Value, typename Result, typename Op>
Result foldr(Op op, Result const& init, ConsStreamAsync<Value> const& streamAsync) {
  if (streamAsync.isEmpty()) {
    return init;
  }
  return op(streamAsync.head(), delay(foldr<Value, Result, Op>,
                                 op,
                                 init,
                                 streamAsync.tail()));
}

/*
  concat :: [[a]] -> [a]
  concat xss = foldr (++) [] xss
*/
// Note - copy streamAsyncs, because we're going to reassign to it
template <typename Value>
ConsStreamAsync<Value> concat(ConsStreamAsync<ConsStreamAsync<Value>> streamAsyncs) {
  while (!streamAsyncs.isEmpty() && streamAsyncs.head().isEmpty()) {
    streamAsyncs = streamAsyncs.tail();
  }

  if (streamAsyncs.isEmpty()) {
    return ConsStreamAsync<Value>();
  }

  return foldr(
      static_cast<ConsStreamAsync<Value> (&)(
          ConsStreamAsync<Value> const&, Delay<ConsStreamAsync<Value>> const&)>(append),
      ConsStreamAsync<Value>(),
      streamAsyncs);
}

// Note - copy streamAsyncs, because we're going to reassign to it
template <typename Value>
ConsStreamAsync<Value> join(ConsStreamAsync<ConsStreamAsync<Value>> streamAsyncs) {
  while (!streamAsyncs.isEmpty() && streamAsyncs.head().isEmpty()) {
    streamAsyncs = streamAsyncs.tail();
  }

  if (streamAsyncs.isEmpty()) {
    return ConsStreamAsync<Value>();
  }

  return ConsStreamAsync<Value>([streamAsyncs]() {
      return ConsCell<Value>(streamAsyncs.head().head(),
                             append(streamAsyncs.head().tail(), join(streamAsyncs.tail())));
    });
}

template <typename Value, typename Func>
auto bind(ConsStreamAsync<Value> const& streamAsync, Func const& f)
  -> decltype(f(streamAsync.head())) {
  return join(fmap(streamAsync, f));
}

template <typename Value, typename Func>
auto then(ConsStreamAsync<Value> const& streamAsync, Func const& f) -> decltype(f()) {
  return join(fmap(streamAsync, [f](Value const&) { return f(); }));
}

// Note - copy streamAsyncs, because we're going to reassign to itx
template <typename Value, typename Func>
auto bind2(ConsStreamAsync<Value> streamAsync, Func const& f)
  -> decltype(f(streamAsync.head())) {
  using M = decltype(bind2(streamAsync, f));

  if (streamAsync.isEmpty()) {
    return M();
  }

  auto y = f(streamAsync.head());
  while (!streamAsync.isEmpty() && y.isEmpty()) {
    streamAsync = streamAsync.tail();
    if (!streamAsync.isEmpty()) {
      y = f(streamAsync.head());
    }
  }

  if (streamAsync.isEmpty()) {
    return M();
  }

  return M([y, streamAsync, f]() {
      using T=decltype(y.head());
      return ConsCell<T>(y.head(),
                         append(y.tail(),
                                bind2(streamAsync.tail(), f)));
    });

}

template <typename Value, typename Func>
auto then2(ConsStreamAsync<Value> const& streamAsync, Func const& f) -> decltype(f()) {
  return bind2(streamAsync, [f](Value const&) { return f(); });
}

template <typename Value>
ConsStreamAsync<Value> join2(ConsStreamAsync<ConsStreamAsync<Value>> streamAsyncs) {
  return bind2(streamAsyncs, [](auto&& v){return std::forward<decltype(v)>(v);});
}

using Unit = std::tuple<>;

ConsStreamAsync<Unit> guardAsync(bool b) {
  if (b) {
    return ConsStreamAsync<Unit>(Unit());
  } else {
    return ConsStreamAsync<Unit>();
  }
}

// dot: :: (b -> c) -> (a -> b) -> (a -> c)
template<typename FuncF, typename FuncG>
auto dot(FuncF&& f, FuncG&& g) {
  return [f = std::forward<FuncF>(f),
          g = std::forward<FuncG>(g)] (auto&&... xs) {
    return f(g(std::forward<decltype(xs)>(xs)...));
  };
}

//-- | Map a function over a list and concatenate the results.
//concatMap               :: (a -> [b]) -> [a] -> [b]
//concatMap f             =  foldr ((++) . f) []

template <typename Func, typename Value>
auto concatMap(Func&& f,  ConsStreamAsync<Value> const& streamAsync) {
  //  -> ConsStreamAsync<decltype(f(streamAsync.head())::value)> {
    using ResultOf = std::invoke_result_t<Func, Value>;

  auto appendF = [f_ = std::forward<Func>(f)]
    (Value v, Delay<ResultOf> const& s) {
    return append(f_(v), s);
  };

  return foldr(
      appendF,
      ResultOf(),
      streamAsync);
}


// template <typename Value>
// ConsStreamAsync<Value> make_consstreamAsync(Value v) {
//   return ConsStreamAsync<Value>(v);
// }

// Applicative:
// app :: f (a -> b) -> f a -> f b
// fs <*> xs = [f x | f <- fs, x <- xs]
// == concatMap (\f -> concatMap (\x -> [f x]) xs) fs
// == fs >>= (\f ->  xs >>= \x -> return (f x))

template <typename Value, typename Func>
auto app2(ConsStreamAsync<Func> const& funcs, ConsStreamAsync<Value> const& values)
//  -> decltype(funcs.head()(values.head())) {
{
  return concatMap(
      [values](Func const& f){
        return concatMap(
            [f](Value v){
              return make<ConsStreamAsync>(f(v));
            },
            values);
      },
      funcs);
  //  return funcs.head()(values.head());
}
template <typename Value, typename Func>
auto app(ConsStreamAsync<Func> const& funcs, ConsStreamAsync<Value> const& values)
//  -> decltype(funcs.head()(values.head())) {
{
  return bind2(funcs,
               [values](Func const& f){
                 return bind2(values,
                              [f](Value const& v){
                                return make<ConsStreamAsync>(f(v));
                              });
               });
}
#endif
