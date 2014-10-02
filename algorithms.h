#ifndef INCLUDED_ALGORITHMS
#define INCLUDED_ALGORITHMS

#ifndef INCLUDED_RANGE2
#include "range2.h"
#endif

#ifndef INCLUDED_CASSERT
#define INCLUDED_CASSERT
#include <cassert>
#endif

#ifndef INCLUDED_FUNCTIONAL
#define INCLUDED_FUNCTIONAL
#include <functional>
#endif

namespace range2 {

struct TYPE_HIDDEN_VISIBILITY NoInline {};
struct TYPE_HIDDEN_VISIBILITY Inline1 {};
struct TYPE_HIDDEN_VISIBILITY Inline2 {};
struct TYPE_HIDDEN_VISIBILITY Inline4 {};
struct TYPE_HIDDEN_VISIBILITY Inline8 {};


template<typename Op>
// Requires UnaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY DerefOp
{
  Op op;

  template<typename... Iterators>
  // Requires input_type(Op, 0) == ValueType<Iterator>
  ALWAYS_INLINE_HIDDEN auto operator()(Iterators... x) -> decltype( op(*x...) ) {
    return op(*x...);
  }
};

template<typename Op>
ALWAYS_INLINE_HIDDEN DerefOp<Op> make_derefop(Op op) {
  return {op};
}

template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY complement
{
  Op op;

  template<typename... T>
  ALWAYS_INLINE_HIDDEN auto operator()(T&&... t) -> decltype( !op(std::forward<T>(t)...) ) {
    return !op(std::forward<T>(t)...);
  }
};

template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY converse
{
  Op op;

  template<typename T0, typename T1>
  // Requires input_type(Op, 0) == T1
  // input_type(Op, 1) == T0
  ALWAYS_INLINE_HIDDEN auto operator()(T0&& x, T1&& y) -> decltype( op(std::forward<T1>(y), std::forward<T0>(x)) ) {
    return op(std::forward<T1>(y), std::forward<T0>(x));
  }
};

template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY complement_converse
{
  Op op;

  template<typename T0, typename T1>
  // Requires input_type(Op, 0) == T1
  // input_type(Op, 1) == T0
  ALWAYS_INLINE_HIDDEN auto operator()(T0&& x, T1&& y) -> decltype( !op(std::forward<T1>(y), std::forward<T0>(x)) ) {
    return !op(std::forward<T1>(y), std::forward<T0>(x));
  }
};


template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY equivalent
{
  Op op;

  template<typename T0, typename T1>
  // Requires input_type(Op, 0) == T1
  // input_type(Op, 1) == T0
  ALWAYS_INLINE_HIDDEN auto operator()(T0&& x, T1&& y) -> decltype( !op(x, y) && !op(y, x) ) {
    return !op(x, y) && !op(y, x);
  }
};
    
template<typename Op>
ALWAYS_INLINE_HIDDEN equivalent<Op> make_equivalent(Op op) {
    return {op};
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE pair<Op, Range>
for_each_impl(Range r, Op op, NoInline) {
  while (!is_empty(r)) {
    op(get_begin(r));
    r = next(r);
  }
  return range2::make_pair(op, r);
}

template<typename Range, typename Op, typename InliningPreferences>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN typename std::enable_if<!IsInlineable<Range>::value, pair<Op, Range>>::type
for_each_impl(Range r, Op op, InliningPreferences) {
  return for_each_impl(r, op, NoInline{});
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE typename std::enable_if<IsInlineable<Range>::value, pair<Op, Range>>::type
for_each_impl(Range r, Op op, Inline4) {

  auto count = get_count(r);
  auto countBy4 = count/4;
  count -= countBy4*4;
  auto iter = get_begin(r);

  while (decltype(count){0} != countBy4) {
    countBy4 = countBy4 - 1;
    auto iter1 = successor(iter);
    auto iter2 = successor(iter1);
    auto iter3 = successor(iter2);
    op(iter);
    iter = successor(iter3);
    op(iter1);
    op(iter2);
    op(iter3);
  }

  while (decltype(count){0} !=count) {
    count = count - 1;
    op(iter);
    iter = successor(iter);
  }
  return range2::make_pair(op, make_range(iter, get_end(r), 0));
}

template<typename Range, typename Op, typename InliningPreferences>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN auto for_each(Range r, Op op, InliningPreferences p) -> decltype( for_each_impl(add_constant_time_count(r), op, p) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range");

  return for_each_impl(add_constant_time_count(r), op, p);
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN auto for_each(Range r, Op op) -> decltype( for_each_impl(add_constant_time_count(r), op, NoInline{}) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range");

  return for_each_impl(add_constant_time_count(r), op, NoInline{});
}



template<typename Range, typename Pred, typename InliningPreferences>
INLINE typename std::enable_if<IsACountedRange<Range>::value, Range>::type
find_if_impl(Range r, Pred p, InliningPreferences x) {
  auto count = get_count(r);
  auto begin = get_begin(r);
  while (decltype(count){0} != count && !p(begin)) {
    count = count - 1, begin = successor(begin);
  }
  return make_range(begin, get_end(r), count);
}

template<typename Range, typename Pred, typename InliningPreferences>
INLINE typename std::enable_if<!IsACountedRange<Range>::value, Range>::type
find_if_impl(Range r, Pred p, InliningPreferences x) {
  while (!is_empty(r) && !p(get_begin(r))) r = next(r);
  return r;
}

template<typename Range, typename Pred, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto find_if(Range r, Pred p, InliningPreferences x) -> decltype ( find_if_impl(add_constant_time_count(r), p, x) ) {
  return find_if_impl(add_constant_time_count(r), p, x);
}

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto find_if(Range r, Pred p) -> decltype ( find_if_impl(add_constant_time_count(r), p, NoInline{}) ) {
  return find_if_impl(add_constant_time_count(r), p, NoInline{});
}



template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN CountType count_if_impl(Range r, Pred p, CountType c, InliningPreferences x) {
  for_each_impl(r, [&p, &c](RangeIterator<Range> const& x) -> void { if (p(x)) c = c + 1; }, x);
  return c;
}

template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN CountType count_if(Range r, Pred p, CountType c, InliningPreferences x) {
  return count_if_impl(add_constant_time_count(r), p, c, x);
}

template<typename Range, typename Pred, typename CountType>
ALWAYS_INLINE_HIDDEN CountType count_if(Range r, Pred p, CountType c) {
  return count_if_impl(add_constant_time_count(r), p, c, NoInline{});
}


// TODO turn into a lambda?
template<typename Op, typename Func, typename State>
struct TYPE_HIDDEN_VISIBILITY reduce_op
{
  Op op;
  Func func;
  State state;

  template<typename Iterator>
  ALWAYS_INLINE_HIDDEN void operator()(Iterator x) {
    state = op(state, func(x));
  }
};

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_nonempty_impl(Range r, Op op, Func f, InliningPreferences p) {
  assert(!is_empty(r));
  auto operation = reduce_op<Op, Func, RangeValue<Range>>{op, f, *get_begin(r)};
  auto tmp = for_each_impl(next(r), operation, p);
  return range2::make_pair(cmove(tmp.m0.state), cmove(tmp.m1));
}

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto reduce_nonempty(Range r, Op op, Func f, InliningPreferences p) -> decltype( reduce_nonempty_impl(add_constant_time_count(r), op, f, p) ) {
  assert(!is_empty(r));
  return reduce_nonempty_impl(add_constant_time_count(r), op, f, p);
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonempty(Range r, Op op, Func f) -> decltype( reduce_nonempty_impl(add_constant_time_count(r), op, f, NoInline{}) ) {
  assert(!is_empty(r));
  return reduce_nonempty_impl(add_constant_time_count(r), op, f, NoInline{});
}


template<typename Range, typename Op, typename Func, typename InlingPreferences>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_impl(Range r, Op op, Func f, RangeValue<Range> const& z, InlingPreferences p) {
  if (is_empty(r)) return range2::make_pair(z, r);
  return reduce_nonempty_impl(add_constant_time_count(r), op, f, p);
}

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto reduce(Range r, Op op, Func f, RangeValue<Range> const& z, InliningPreferences p) -> decltype( reduce_impl(add_constant_time_count(r), op, f, z, p) ) {
  return reduce_impl(add_constant_time_count(r), op, f, z, p);
}

template<typename Range, typename Op, typename Func>
  ALWAYS_INLINE_HIDDEN auto reduce(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_impl(add_constant_time_count(r), op, f, z, NoInline{}) ) {
  return reduce_impl(add_constant_time_count(r), op, f, z, NoInline{});
}

// TODO turn into a lambda?
template<typename Op, typename Func, typename State>
struct TYPE_HIDDEN_VISIBILITY reduce_nonzeroes_op
{
  Op op;
  Func func;
  State state;
  State const& z;

  template<typename Iterator>
  ALWAYS_INLINE_HIDDEN void operator()(Iterator x) {
    auto tmp = func(x);
    if (z != tmp) state = op(state, func(x));
  }
};

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_nonzeroes_impl(Range r, Op op, Func f, RangeValue<Range> const& z, InliningPreferences p) {
  auto firstNonZero = find_if_impl(r, [&z, &f](RangeIterator<Range> x) -> bool { return z != f(x); }, p);
  if (!is_empty(firstNonZero)) {
    auto tmp = for_each_impl(next(firstNonZero), reduce_nonzeroes_op<Op, Func, RangeValue<Range>>{ op, f, f(get_begin(firstNonZero)), z}, p);
    return range2::make_pair(tmp.m0.state, tmp.m1);
  } else {
    return range2::make_pair(z, r);
  }
}

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto reduce_nonzeroes(Range r, Op op, Func f, RangeValue<Range> const& z, InliningPreferences p) -> decltype( reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z, p) ) {
  return reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z, p);
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonzeroes(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z, NoInline{}) ) {
  return reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z, NoInline{});
}


template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<Range0>::value && !IsAFiniteRange<Range1>::value, pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  auto iter0 = get_begin(r0);
  auto iter1 = get_begin(r1);
  while (rel(iter0, iter1)) {
    iter0 = successor(iter0), iter1 = successor(iter1);
  }
  return range2::make_pair(make_range(iter0, NotPresent{}, NotPresent{}), make_range(iter1, NotPresent{}, NotPresent{}));
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<Range0>::value && !IsAFiniteRange<Range1>::value, pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  auto iter1 = get_begin(r1);
  while (!is_empty(r0) && rel(get_begin(r0), iter1)) {
    r0 = next(r0), iter1 = successor(iter1);
  }
  return range2::make_pair(r0, make_range(iter1, NotPresent{}, NotPresent{}));
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<Range0>::value && IsAFiniteRange<Range1>::value, pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  auto iter0 = get_begin(r0);
  while (!is_empty(r1) && rel(iter0, get_begin(r1))) {
    iter0 = successor(iter0), r1 = next(r1);
  }
  return range2::make_pair(make_range(iter0, NotPresent{}, NotPresent{}), r1);
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<Range0>::value && IsAFiniteRange<Range1>::value && !(IsACountedRange<Range0>::value && IsACountedRange<Range1>::value), pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  while (!is_empty(r0) &&
         !is_empty(r1) &&
         rel(get_begin(r0), get_begin(r1))) {
    r0 = next(r0), r1 = next(r1);
  }
  return range2::make_pair(r0, r1);
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
INLINE typename std::enable_if<IsACountedRange<Range0>::value && IsACountedRange<Range1>::value, pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  if (get_count(r1) > get_count(r0)) {
    auto diff = get_count(r1) - get_count(r0); // +ve
    auto tmp = find_mismatch_impl(r0, make_range(get_begin(r1), NotPresent{}, NotPresent{}), rel, p);
    return range2::make_pair(tmp.m0, make_range(get_begin(tmp.m1), get_end(r1), diff + get_count(tmp.m0)));
  } else {
    auto diff = get_count(r0) - get_count(r1); // 0 or +ve
    auto tmp = find_mismatch_impl(make_range(get_begin(r0), NotPresent{}, NotPresent{}), r1, rel, p);
    return range2::make_pair(make_range(get_begin(tmp.m0), get_end(r0), diff + get_count(tmp.m1)), tmp.m1);
  }
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto find_mismatch(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) -> decltype( find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, NoInline{}) ) {
  return find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, p);
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_mismatch(Range0 r0, Range1 r1, Rel rel) -> decltype( find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, NoInline{}) ) {
  return find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, NoInline{});
}


template<typename Range, typename Rel, typename InliningPreferences>
INLINE pair<RangeValue<Range>, Range> find_adjacent_mismatch_input_non_empty_impl(Range r, Rel rel, InliningPreferences p) {
  assert(!is_empty(r));
  RangeValue<Range> tmp = *get_begin(r);
  r = next(r);
  while (!is_empty(r)) {
    if (!rel(&tmp, get_begin(r))) return range2::make_pair(tmp, r);
    tmp = *get_begin(r);
    r = next(r);
  }
  return range2::make_pair(tmp, r);
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch_input_non_empty(Range r, Rel rel, InliningPreferences p) -> decltype( find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel, p) ) {
   return find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch_input_non_empty(Range r, Rel rel) -> decltype( find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel, NoInline{}) ) {
  return find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel, NoInline{});
}



template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN typename std::enable_if<!IsAFiniteRange<Range>::value, Range>::type
find_adjacent_mismatch_impl(Range r, Rel rel, InliningPreferences p) {
  auto begin = get_begin(r);
  auto prev = begin;
  do {
    prev = begin;
    begin = successor(begin);
  } while (rel(prev, begin));
  return make_range(begin, NotPresent{}, NotPresent{});
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN typename std::enable_if<IsAFiniteRange<Range>::value && !IsACountedRange<Range>::value, Range>::type
find_adjacent_mismatch_impl(Range r, Rel rel, InliningPreferences p) {
  if (is_empty(r)) return r;
  auto begin = get_begin(r);
  auto prev = begin;
  auto end = get_end(r);
  do {
    prev = begin;
    begin = successor(begin);
  } while ((begin != end) && rel(prev, begin));
  return make_range(begin, end, NotPresent{});
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN typename std::enable_if<IsACountedRange<Range>::value, Range>::type
find_adjacent_mismatch_impl(Range r, Rel rel, InliningPreferences p) {
  if (is_empty(r)) return r;
  auto begin = get_begin(r);
  auto prev = begin;
  auto count = get_count(r);
  do {
    prev = begin;
    begin = successor(begin), count = count - 1;
  } while (decltype(count){0} != count && rel(prev, begin));
  return make_range(begin, get_end(r), count);
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch(Range r, Rel rel, InliningPreferences p) -> decltype( find_adjacent_mismatch_impl(add_constant_time_count(r), rel, p) ) {
  return find_adjacent_mismatch_impl(add_constant_time_count(r), rel, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch(Range r, Rel rel) -> decltype( find_adjacent_mismatch_impl(add_constant_time_count(r), rel, NoInline{}) ) {
  return find_adjacent_mismatch_impl(add_constant_time_count(r), rel, NoInline{});
}


template<typename Range, typename Rel,  typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool relation_preserving_impl(Range r, Rel rel, InliningPreferences p, std::input_iterator_tag) {
  if (is_empty(r)) return true;
  return is_empty(find_adjacent_mismatch_input_non_empty(r, rel, p).m1);
}

template<typename Range, typename Rel,  typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool relation_preserving_impl(Range r, Rel rel, InliningPreferences p, std::forward_iterator_tag) {
  return is_empty(find_adjacent_mismatch_impl(r, rel, p));
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool relation_preserving(Range r, Rel rel, InliningPreferences p) {
  return relation_preserving_impl(add_constant_time_count(r), rel, p, get_category(r));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool relation_preserving(Range r, Rel rel) {
  return relation_preserving_impl(add_constant_time_count(r), rel, NoInline{}, get_category(r));
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool strictly_increasing_range(Range r, Rel rel, InliningPreferences p) {
  return relation_preserving(r, rel, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool strictly_increasing_range(Range r, Rel rel) {
  return relation_preserving(r, rel, NoInline{});
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool increasing_range(Range r, Rel rel, InliningPreferences p) {
  return relation_preserving(r, complement_converse<Rel>{rel}, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool increasing_range(Range r, Rel rel) {
  return relation_preserving(r, complement_converse<Rel>{rel}, NoInline{});
}


template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool partitioned_impl(Range r, Rel rel, InliningPreferences p) {
  return is_empty(find_if_impl(find_if_impl(r, rel, p), complement<Rel>{rel}, p));
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool partitioned(Range r, Rel rel, InliningPreferences p) {
  return partitioned_impl(add_constant_time_count(r), rel, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool partitioned(Range r, Rel rel) {
  return partitioned_impl(add_constant_time_count(r), rel, NoInline{});
}

namespace impl {

template<unsigned int Divisor>
struct TYPE_HIDDEN_VISIBILITY Divide {
  // Precondition x > 0; postcondition 0 <= returnValue < x
  template<typename T>
  ALWAYS_INLINE_HIDDEN T operator()(T x) const { return x/Divisor; }
};

typedef Divide<2> Halve;

} // namespace impl

template<typename Rng, RangeDifferenceType<Rng> LinearSearchLimit, typename Pred, typename InliningPreferences, typename BisectionOperation>
// requires BisectionOperation return a value < the value passed in, and is also >= 0.
ALWAYS_INLINE_HIDDEN pair<Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, typename GetEnd<Rng>::type, Present>> bisecting_search(Rng r, Pred pred, BisectionOperation bo, InliningPreferences p) {
  auto iter = get_begin(r);
  auto n = get_count(r);
  auto lhsN = decltype(n)(0);

  while (decltype(n){0} != n) {
    if (n <= LinearSearchLimit) {
      auto tmp = find_if_impl(make_range(iter, NotPresent{}, n), pred, p);
      lhsN = lhsN + (n - get_count(tmp));
      iter = get_begin(tmp);
      break;
    }
    auto h = bo(n);
    auto m = range2::advance(iter, h);
    if (pred(m)) {
      n = h;
    } else {
      lhsN = lhsN + h + 1,
      iter = successor(m),
      n = n - (h + 1);
    }
  }
  return range2::make_pair(make_range(get_begin(r), iter, lhsN), make_range(iter, get_end(r), get_count(r) - lhsN));
}

template<typename Range, typename Pred, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto partition_point_impl(Range r, Pred pred, InliningPreferences p) -> decltype( bisecting_search<Range, 0>(r, pred, impl::Halve{}, p) ) {
  return bisecting_search<Range, 0>(r, pred, impl::Halve{}, p);
}

template<typename Range, typename Pred, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto partition_point(Range r, Pred pred, InliningPreferences p) -> decltype( partition_point_impl(add_linear_time_count(r), pred, p) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range to perform binary search");
  return partition_point_impl(add_linear_time_count(r), pred, p);
}

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto partition_point(Range r, Pred pred) -> decltype( partition_point_impl(add_linear_time_count(r), pred, NoInline{}) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range to perform binary search");
  return partition_point_impl(add_linear_time_count(r), pred, NoInline{});
}

namespace impl {

template<typename Value, typename Rel>
struct TYPE_HIDDEN_VISIBILITY lower_bound_pred {
  Value const& v;
  Rel rel;

  bool ALWAYS_INLINE_HIDDEN operator()(Value const& x) {
    return !rel(x, v);
  }
};

template<typename Value, typename Rel>
struct TYPE_HIDDEN_VISIBILITY upper_bound_pred {
  Value const& v;
  Rel rel;

  bool ALWAYS_INLINE_HIDDEN operator()(Value const& x) {
    return rel(v, x);
  }
};

} // namespace impl

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto lower_bound_predicate_impl(Range r, Rel rel, RangeValue<Range> const& a, InliningPreferences p) -> decltype ( partition_point_impl(r, make_derefop(impl::lower_bound_pred<RangeValue<Range>, Rel>{a, rel}), p) ) {
  return partition_point_impl(r, make_derefop(impl::lower_bound_pred<RangeValue<Range>, Rel>{a, rel}), p);
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto lower_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a, InliningPreferences p) -> decltype( lower_bound_predicate_impl(add_linear_time_count(r), rel, a, p) ) {
  return lower_bound_predicate_impl(add_linear_time_count(r), rel, a, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto lower_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( lower_bound_predicate_impl(add_linear_time_count(r), rel, a, NoInline{}) ) {
  return lower_bound_predicate_impl(add_linear_time_count(r), rel, a, NoInline{});
}


template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto upper_bound_predicate_impl(Range r, Rel rel, RangeValue<Range> const& a, InliningPreferences p) -> decltype ( partition_point_impl(r, make_derefop(impl::upper_bound_pred<RangeValue<Range>, Rel>{a, rel}), p) ){
  return partition_point_impl(r, make_derefop(impl::upper_bound_pred<RangeValue<Range>, Rel>{a, rel}), p);
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto upper_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a, InliningPreferences p) -> decltype( upper_bound_predicate_impl(add_linear_time_count(r), rel, a, p) ) {
  return upper_bound_predicate_impl(add_linear_time_count(r), rel, a, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto upper_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( upper_bound_predicate_impl(add_linear_time_count(r), rel, a, NoInline{}) ) {
  return upper_bound_predicate_impl(add_linear_time_count(r), rel, a, NoInline{});
}

template<typename Rng, typename Rel, typename BisectionOperation, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN triple<Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, typename GetEnd<Rng>::type, Present>>
equivalent_range_impl(Rng r, Rel rel, RangeValue<Rng> const& a, BisectionOperation bo, InliningPreferences p) {
  auto iter = get_begin(r);
  auto n = get_count(r);
  auto lhsN = decltype(n)(0);

  while (decltype(n){0} != n) {
    auto h = bo(n);
    auto m = range2::advance(iter, h);
    if (rel(*m, a)) {
      lhsN = lhsN + h + 1;
      iter = successor(m);
      n =  n - (h + 1);
    } else if (rel(a, *m)) {
      // m is greater than the equivalent range, so shrink the search range.
      n = h;
    } else {
      // m is somewhere in the middle of the equivalent range
      auto lower_bound = lower_bound_predicate_impl(make_range(iter, NotPresent{}, n), rel, a, p);
      auto upper_bound = upper_bound_predicate_impl(lower_bound.m1, rel, a, p);

      return make_triple(make_range(get_begin(r), get_begin(lower_bound.m1), get_count(lower_bound.m0) + lhsN),
       upper_bound.m0,
       make_range(get_begin(upper_bound.m1), get_end(r), get_count(r) - get_count(upper_bound.m0) - get_count(lower_bound.m0) - lhsN));
    }
  }
  return make_triple(make_range(get_begin(r), iter, lhsN), make_range(iter, iter, 0), make_range(iter, get_end(r), get_count(r) - lhsN));
}

template<typename Range, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto equivalent_range(Range r, Rel rel, RangeValue<Range> const& a, InliningPreferences p) -> decltype( equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{}, p) ) {
  return equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{}, p);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto equivalent_range(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{}, NoInline{}) ) {
  return equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{}, NoInline{});
}



template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_equivalent_impl2(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  auto tmp = find_mismatch_impl(r0, r1, rel, p);
  return is_empty(tmp.m0) && is_empty(tmp.m1);
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN typename std::enable_if<IsACountedRange<Range0>::value && IsACountedRange<Range1>::value, bool>::type
lexicographical_equivalent_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  if (get_count(r0) != get_count(r1)) {
    return false ;
  } else {
    return lexicographical_equivalent_impl2(r0, r1, rel, p);
  }
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN typename std::enable_if<!IsACountedRange<Range0>::value || !IsACountedRange<Range1>::value, bool>::type
lexicographical_equivalent_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  return lexicographical_equivalent_impl2(r0, r1, rel, p);
}


template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_equivalent(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  return lexicographical_equivalent_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, p);
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_equivalent(Range0 r0, Range1 r1, Rel rel) {
  return lexicographical_equivalent_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, NoInline{});
}

template<typename Range0, typename Range1, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_equal(Range0 r0, Range1 r1, InliningPreferences p) {
  static_assert(std::is_same<RangeValue<Range0>, RangeValue<Range1>>::value, "Both ranges must have the same value type");
  return lexicographical_equivalent(r0, r1, make_refefop(std::equal_to<RangeValue<Range0>>{}), p);
}

template<typename Range0, typename Range1>
ALWAYS_INLINE_HIDDEN bool lexicographical_equal(Range0 r0, Range1 r1) {
  static_assert(std::is_same<RangeValue<Range0>, RangeValue<Range1>>::value, "Both ranges must have the same value type");
  return lexicographical_equivalent(r0, r1, make_derefop(std::equal_to<RangeValue<Range0>>{}));
}


template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_compare_impl(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  auto tmp = find_mismatch_impl(r0, r1, make_derefop(make_equivalent(rel)), p);
  if (is_empty(tmp.m1)) {
    return false;
  } else if (is_empty(tmp.m0)) {
    return true;
  } else {
    // Therefore *get_begin(tmp.m0) is not equivalent to *get_begin(tmp.m1).
    return rel(*get_begin(tmp.m0), *get_begin(tmp.m1));
  }
}

template<typename Range0, typename Range1, typename Rel, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_compare(Range0 r0, Range1 r1, Rel rel, InliningPreferences p) {
  return lexicographical_compare_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, p);
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_compare(Range0 r0, Range1 r1, Rel rel) {
  return lexicographical_compare_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel, NoInline{});
}


template<typename Range0, typename Range1, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN bool lexicographical_less(Range0 r0, Range1 r1, InliningPreferences p) {
  static_assert(std::is_same<RangeValue<Range0>, RangeValue<Range1>>::value, "Both ranges must have the same value type");
  return lexicographical_compare(r0, r1, std::less<RangeValue<Range0>>{}, p);
}

template<typename Range0, typename Range1>
ALWAYS_INLINE_HIDDEN bool lexicographical_less(Range0 r0, Range1 r1) {
  static_assert(std::is_same<RangeValue<Range0>, RangeValue<Range1>>::value, "Both ranges must have the same value type");
  return lexicographical_compare(r0, r1, std::less<RangeValue<Range0>>{});
}

struct TYPE_DEFAULT_VISIBILITY copy_step
{
  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) const {
    sink(get_begin(o), *get_begin(i));
    i = next(i), o = next(o);
  }
};

struct TYPE_DEFAULT_VISIBILITY move_step
{
  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) const {
    sink(get_begin(o), std::move(*get_begin(i)));
    i = next(i), o = next(o);
  }
};

struct TYPE_DEFAULT_VISIBILITY swap_step
{
  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) const {
    using std::swap;
    swap(*get_begin(i), *get_begin(o));
    i = next(i), o = next(o);
  }
};

template<typename Pred, typename Step>
struct TYPE_DEFAULT_VISIBILITY step_if
{
  Pred p;
  Step step;

  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) {
    if (p(get_begin(i))) {
      step(i, o) ;
    } else {
      i = next(i);
    }
  }
};

template<typename Pred, typename Step>
ALWAYS_INLINE_HIDDEN step_if<Pred, Step> make_step_if(Pred p, Step s) {
  return {p, s};
}

template<typename R0, typename R1, typename Step, typename InliningPreferences>
INLINE pair<R0, R1>
visit_2_ranges_impl(R0 r0, R1 r1, Step step, InliningPreferences p) {
  while(!is_empty(r0) && !is_empty(r1)) step(r0, r1);
  return make_pair(r0, r1);
}

/*
template<typename R0, typename R1, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value, pair<R0, R1> >::type
visit_2_ranges_impl(R0 r0, R1 r1, Step step, InliningPreferences p) {
  while(!is_empty(r0) && !is_empty(r1)) step(r0, r1);
  return make_pair(r0, r1);
}

template<typename R0, typename R1, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && !IsAFiniteRange<R1>::value, pair<R0, R1> >::type
visit_2_ranges_impl(R0 r0, R1 r1, Step step, InliningPreferences p) {
  while(!is_empty(r0)) step(r0, r1);
  return make_pair(r0, r1);
}

template<typename R0, typename R1, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value, pair<R0, R1> >::type
visit_2_ranges_impl(R0 r0, R1 r1, Step step, InliningPreferences p) {
  while(!is_empty(r1)) step(r0, r1);
  return make_pair(r0, r1);
}
*/

template<typename R0, typename R1, typename Step, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto visit_2_ranges(R0 r0, R1 r1, Step step, InliningPreferences p) -> decltype ( visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step, p) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value, "One of the ranges must be finite");
  return visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step, p);
}

template<typename R0, typename R1, typename Step>
ALWAYS_INLINE_HIDDEN auto visit_2_ranges(R0 r0, R1 r1, Step step) -> decltype ( visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step, NoInline{}) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value, "One of the ranges must be finite");
  return visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step, NoInline{});
}

template<typename Pred, typename Step>
struct TYPE_HIDDEN_VISIBILITY split_if
{
  Pred p;
  Step step;
  template<typename I, typename Of, typename Ot>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, Of& of, Ot& ot) {
    if (p(get_begin(i))) {
      step(i, ot);
    } else {
      step(i, of);
    }
  }
};

template<typename Pred, typename Step>
ALWAYS_INLINE_HIDDEN split_if<Pred, Step> make_split_if(Pred p, Step s) {
  return {p, s};
}


template<typename Rel, typename Step>
struct TYPE_HIDDEN_VISIBILITY merge_if
{
  Rel rel;
  Step step;
  template<typename I0, typename I1, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I0& i0, I1& i1, O& o) {
    if (rel(*get_begin(i1), *get_begin(i0))) {
      step(i1, o);
    } else {
      step(i0, o);
    }
  }
};

template<typename Rel, typename Step>
ALWAYS_INLINE_HIDDEN merge_if<Rel, Step> make_merge_if(Rel r, Step s) {
  return {r, s};
}


template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE triple<R0, R1, R2>
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r0) && !is_empty(r1) && !is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

/*
template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value && IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r0) && !is_empty(r1) && !is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value && IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r1) && !is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && !IsAFiniteRange<R1>::value && IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r0) && !is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value && !IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r0) && !is_empty(r1)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<R0>::value && !IsAFiniteRange<R1>::value && IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<!IsAFiniteRange<R0>::value && IsAFiniteRange<R1>::value && !IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r1)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
INLINE typename std::enable_if<IsAFiniteRange<R0>::value && !IsAFiniteRange<R1>::value && !IsAFiniteRange<R2>::value, triple<R0, R1, R2>>::type
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) {
  while (!is_empty(r0)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}
*/

template<typename R0, typename R1, typename R2, typename Step, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto visit_3_ranges(R0 r0, R1 r1, R2 r2, Step step, InliningPreferences p) -> decltype ( visit_3_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), add_constant_time_count(r2), step, p) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value || IsAFiniteRange<R2>::value, "One of the ranges must be finite");
  return visit_3_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), add_constant_time_count(r2), step, p);
}

template<typename R0, typename R1, typename R2, typename Step>
ALWAYS_INLINE_HIDDEN auto visit_3_ranges(R0 r0, R1 r1, R2 r2, Step step) -> decltype ( visit_3_ranges_impl (add_constant_time_count(r0),add_constant_time_count(r1), add_constant_time_count(r2), step, NoInline{}) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value || IsAFiniteRange<R2>::value, "One of the ranges must be finite");
  return visit_3_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), add_constant_time_count(r2), step, NoInline{});
}

} // namespace range2

#endif
