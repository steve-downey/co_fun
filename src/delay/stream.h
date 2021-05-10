// stream.h                                                           -*-C++-*-
#ifndef INCLUDED_STREAM
#define INCLUDED_STREAM

#include <delay/delay.h>
#include <optional>
#include <iterator>
#include <memory>
#include <tuple>

template <typename Value>
class ConsStream;

template <typename Value>
class ConsStreamIterator;

template <typename Value>
class ConsCell {
  Value head_;
  ConsStream<Value> tail_;

  friend class ConsStreamIterator<Value>;

public:
  ConsCell(Value const& v, ConsStream<Value> const& stream)
      : head_(v), tail_(stream) {
  }

  explicit ConsCell(Value const& v) : head_(v), tail_() {
  }

  Value const& head() const {
    return head_;
  }

  ConsStream<Value> const& tail() const {
    return tail_;
  }
};

template <typename Value>
class ConsStream {
  std::shared_ptr<Delay<ConsCell<Value>>> delayed_cell_;

  friend class ConsStreamIterator<Value>;

public:
  typedef Value value;

  ConsStream() = default;

  ConsStream(Value const& value)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(
            [value]() { return ConsCell<Value>(value); })) {
  }

  ConsStream(Value && value)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(
                          [v = std::forward<Value>(value)]() { return ConsCell<Value>(v); })) {
  }

  template <typename Func,
            typename = typename std::enable_if<
                !std::is_convertible<Func, ConsStream>::value>::type>
  ConsStream(Func&& f)
      : delayed_cell_(std::make_shared<Delay<ConsCell<Value>>>(f)) {
  }

  bool isEmpty() const {
    return !delayed_cell_;
  }

  Value head() const {
    return force(*delayed_cell_).head();
  }

  ConsStream<Value> tail() const {
    return force(*delayed_cell_).tail();
  }

  typedef ConsStreamIterator<Value> iterator;

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
ConsStream<Value> make_stream(Value v) {
  return ConsStream<Value>([v]() { return ConsCell<Value>(v); });
}


template <template<typename> typename Applicative, typename Value>
struct Make {
  Applicative<Value> operator()(Value const& v);
  Applicative<Value> operator()(Value && v);
};

template<typename Value>
struct Make<ConsStream, Value> {
  typedef typename std::decay<Value>::type V;
  ConsStream<V> operator()(Value const& v) {
    // return ConsStream<Value>([v]() { return ConsCell<Value>(v); });
    return ConsStream<V>(v);
  }
  ConsStream<V> operator()(V && v) {
    // return ConsStream<Value>([v]() { return ConsCell<Value>(v); });
    return ConsStream<V>(v);
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
class ConsStreamIterator : public std::iterator<std::forward_iterator_tag,
                                                std::remove_cv_t<Value>,
                                                std::ptrdiff_t,
                                                Value*,
                                                Value&> {
  std::shared_ptr<Delay<ConsCell<Value>>> delayed_cell_;

  explicit ConsStreamIterator(std::shared_ptr<Delay<ConsCell<Value>>> cell)
      : delayed_cell_(cell) {
  }

  friend class ConsStream<Value>;

public:
  ConsStreamIterator() = default; // Default construct gives end.

  void swap(ConsStreamIterator& other) noexcept {
    using std::swap;
    swap(delayed_cell_, other.delayed_cell_);
  }

  ConsStreamIterator& operator++() // Pre-increment
  {
    delayed_cell_ = force(*delayed_cell_).tail().delayed_cell_;
    return *this;
  }

  ConsStreamIterator operator++(int) // Post-increment
  {
    ConsStreamIterator tmp(*this);
    delayed_cell_ = force(*delayed_cell_).tail().delayed_cell_;
    return tmp;
  }

  // two-way comparison: v.begin() == v.cbegin() and vice versa
  template <class OtherType>
  bool operator==(const ConsStreamIterator<OtherType>& rhs) const {
    return delayed_cell_ == rhs.delayed_cell_;
  }

  template <class OtherType>
  bool operator!=(const ConsStreamIterator<OtherType>& rhs) const {
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
ConsStream<Value> cons(Value n, ConsStream<Value> const& stream) {
  return ConsStream<Value>(
      [n, stream]() { return ConsCell<Value>(n, stream); });
}

template <typename Value>
Value last(ConsStream<Value> const& stream) {
  ConsStream<Value> s = stream;
  while (!s.tail().isEmpty()) {
    s = s.tail();
  }
  return s.head();
}

template<typename Value>
ConsStream<Value> init(ConsStream<Value> const& stream) {
  if (stream.tail().isEmpty()) {
    return ConsStream<Value>();
  }
  return cons(stream.head(), init(stream.tail()));
}

template <typename Value>
size_t lengthAcc(ConsStream<Value> const& stream, size_t n) {
  if (stream.isEmpty()) {
    return n;
  }
  return lengthAcc(stream.tail(), n+1);
}

template <typename Value>
size_t length(ConsStream<Value> const& stream) {
  return lengthAcc(stream, 0);
}

template<typename Value, typename Predicate>
ConsStream<Value> filter(Predicate const& p, ConsStream<Value> stream) {
  while (!stream.isEmpty() && !p(stream.head())) {
    stream = stream.tail();
  }

  if (stream.isEmpty()) {
    return ConsStream<Value>();
  }

  return ConsStream<Value>(
      [p, stream]() {
        return ConsCell<Value>(stream.head(), filter(p, stream.tail()));
      });
}

template <typename Value>
ConsStream<Value> rangeFrom(Value n, Value m) {
  if (n > m) {
    return ConsStream<Value>();
  }
  return ConsStream<Value>(
      [n, m]() { return ConsCell<Value>(n, rangeFrom(n + 1, m)); });
}

template <typename Value>
ConsStream<Value> iota(Value n = Value()) {
  return ConsStream<Value>([n]() { return ConsCell<Value>(n, iota(n + 1)); });
}

template <typename Value>
ConsStream<Value> take(ConsStream<Value> const& strm, int n) {
  if (n == 0 || strm.isEmpty()) {
    return ConsStream<Value>();
  }
  return ConsStream<Value>([strm, n]() {
    return ConsCell<Value>(strm.head(), take(strm.tail(), n - 1));
  });
}

template <typename Value>
ConsStream<Value> drop(ConsStream<Value> const& strm, int n) {
  if (strm.isEmpty()) {
    return ConsStream<Value>();
  }

  if (n == 0) {
    return strm;
  }

  return drop(strm.tail(), n-1);
}

template <typename Value>
ConsStream<Value> append(ConsStream<Value> const& first,
                         ConsStream<Value> const& second) {
  if (first.isEmpty()) {
    return second;
  }
  return ConsStream<Value>([first, second]() {
    return ConsCell<Value>(first.head(), append(first.tail(), second));
  });
}

template <typename Value>
ConsStream<Value> append(ConsStream<Value> const& first,
                         Delay<ConsStream<Value>> const& second) {
  if (first.isEmpty()) {
    return force(second);
  }
  return ConsStream<Value>([first, second]() {
      return ConsCell<Value>(first.head(), append(first.tail(), second));
    });
}

template <typename Value, typename Func>
auto fmap(ConsStream<Value> const& stream, Func const& f)
    -> ConsStream<decltype(f(stream.head()))> {
  using Mapped = decltype(f(stream.head()));
  if (stream.isEmpty()) {
    return ConsStream<Mapped>();
  }

  return ConsStream<Mapped>([stream, f]() {
    return ConsCell<Mapped>(f(stream.head()), fmap(stream.tail(), f));
  });
}

/*
  foldr            :: (a -> b -> b) -> b -> [a] -> b
  foldr f z []     =  z
  foldr f z (x:xs) =  f x (foldr f z xs)
*/
template <typename Value, typename Result, typename Op>
Result foldr(Op op, Result const& init, ConsStream<Value> const& stream) {
  if (stream.isEmpty()) {
    return init;
  }
  return op(stream.head(), delay(foldr<Value, Result, Op>,
                                 op,
                                 init,
                                 stream.tail()));
}

/*
  concat :: [[a]] -> [a]
  concat xss = foldr (++) [] xss
*/
// Note - copy streams, because we're going to reassign to it
template <typename Value>
ConsStream<Value> concat(ConsStream<ConsStream<Value>> streams) {
  while (!streams.isEmpty() && streams.head().isEmpty()) {
    streams = streams.tail();
  }

  if (streams.isEmpty()) {
    return ConsStream<Value>();
  }

  return foldr(
      static_cast<ConsStream<Value> (&)(
          ConsStream<Value> const&, Delay<ConsStream<Value>> const&)>(append),
      ConsStream<Value>(),
      streams);
}

// Note - copy streams, because we're going to reassign to it
template <typename Value>
ConsStream<Value> join(ConsStream<ConsStream<Value>> streams) {
  while (!streams.isEmpty() && streams.head().isEmpty()) {
    streams = streams.tail();
  }

  if (streams.isEmpty()) {
    return ConsStream<Value>();
  }

  return ConsStream<Value>([streams]() {
    return ConsCell<Value>(streams.head().head(),
                           append(streams.head().tail(), join(streams.tail())));
  });
}

template <typename Value, typename Func>
auto bind(ConsStream<Value> const& stream, Func const& f)
    -> decltype(f(stream.head())) {
  return join(fmap(stream, f));
}

template <typename Value, typename Func>
auto then(ConsStream<Value> const& stream, Func const& f) -> decltype(f()) {
  return join(fmap(stream, [f](Value const&) { return f(); }));
}

// Note - copy streams, because we're going to reassign to itx
template <typename Value, typename Func>
auto bind2(ConsStream<Value> stream, Func const& f)
  -> decltype(f(stream.head())) {
  using M = decltype(bind2(stream, f));

  if (stream.isEmpty()) {
    return M();
  }

  auto y = f(stream.head());
  while (!stream.isEmpty() && y.isEmpty()) {
    stream = stream.tail();
    if (!stream.isEmpty()) {
      y = f(stream.head());
    }
  }

  if (stream.isEmpty()) {
    return M();
  }

  return M([y, stream, f]() {
      using T=decltype(y.head());
      return ConsCell<T>(y.head(),
                         append(y.tail(),
                                bind2(stream.tail(), f)));
    });

}

template <typename Value, typename Func>
auto then2(ConsStream<Value> const& stream, Func const& f) -> decltype(f()) {
  return bind2(stream, [f](Value const&) { return f(); });
}

template <typename Value>
ConsStream<Value> join2(ConsStream<ConsStream<Value>> streams) {
  return bind2(streams, [](auto&& v){return std::forward<decltype(v)>(v);});
}

using Unit = std::tuple<>;

ConsStream<Unit> guard(bool b) {
  if (b) {
    return ConsStream<Unit>(Unit());
  } else {
    return ConsStream<Unit>();
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
auto concatMap(Func&& f,  ConsStream<Value> const& stream) {
  //  -> ConsStream<decltype(f(stream.head())::value)> {
  using ResultOf = std::result_of_t<Func(Value)>;

  auto appendF = [f_ = std::forward<Func>(f)]
    (Value v, Delay<ResultOf> const& s) {
    return append(f_(v), s);
  };

  return foldr(
      appendF,
      ResultOf(),
      stream);
}


// template <typename Value>
// ConsStream<Value> make_consstream(Value v) {
//   return ConsStream<Value>(v);
// }

// Applicative:
// app :: f (a -> b) -> f a -> f b
// fs <*> xs = [f x | f <- fs, x <- xs]
// == concatMap (\f -> concatMap (\x -> [f x]) xs) fs
// == fs >>= (\f ->  xs >>= \x -> return (f x))

template <typename Value, typename Func>
auto app2(ConsStream<Func> const& funcs, ConsStream<Value> const& values)
//  -> decltype(funcs.head()(values.head())) {
{
  return concatMap(
      [values](Func const& f){
        return concatMap(
            [f](Value v){
              return make<ConsStream>(f(v));
            },
            values);
      },
      funcs);
  //  return funcs.head()(values.head());
}
template <typename Value, typename Func>
auto app(ConsStream<Func> const& funcs, ConsStream<Value> const& values)
//  -> decltype(funcs.head()(values.head())) {
{
  return bind2(funcs,
               [values](Func const& f){
                 return bind2(values,
                              [f](Value const& v){
                                return make<ConsStream>(f(v));
                              });
               });
}
#endif
