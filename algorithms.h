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

template<typename Op>
// Requires UnaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY deref_op
{
  Op op;

  template<typename... Iterators>
  // Requires input_type(Op, 0) == ValueType<Iterator>
  ALWAYS_INLINE_HIDDEN auto operator()(Iterators... x) -> decltype( op(*x...) ) {
    return op(*x...);
  }
};

template<typename Op>
ALWAYS_INLINE_HIDDEN deref_op<Op> make_derefop(Op op) {
  return {cmove(op)};
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
ALWAYS_INLINE_HIDDEN complement<Op> make_complement(Op op) {
  return {cmove(op)};
}


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
ALWAYS_INLINE_HIDDEN converse<Op> make_converse(Op op) {
  return {cmove(op)};
}


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
ALWAYS_INLINE_HIDDEN complement_converse<Op> make_complement_converse(Op op) {
  return {cmove(op)};
}


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
  return {cmove(op)};
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE pair<Op, Range>
for_each_impl(Range r, Op op) {
  while (!is_empty(r)) {
    op(get_begin(r));
    r = successor(r);
  }
  return range2::make_pair(op, r);
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN auto for_each(Range r, Op op) -> decltype( for_each_impl(add_constant_time_count(r), op) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range");

  return for_each_impl(add_constant_time_count(r), op);
}


template<typename Range, typename Pred>
INLINE Range
find_if_impl(Range r, Pred p) {
  while (!is_empty(r) && !p(get_begin(r))) r = successor(r);
  return r;
}

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto find_if(Range r, Pred p) -> decltype ( find_if_impl(add_constant_time_count(r), p) ) {
  return find_if_impl(add_constant_time_count(r), p);
}



template<typename Range, typename Pred, typename CountType>
ALWAYS_INLINE_HIDDEN CountType count_if_impl(Range r, Pred p, CountType c) {
  for_each_impl(r, [&p, &c](RangeIterator<Range> const& x) -> void { if (p(x)) c = c + 1; });
  return c;
}

template<typename Range, typename Pred, typename CountType>
ALWAYS_INLINE_HIDDEN CountType count_if(Range r, Pred p, CountType c) {
  return count_if_impl(add_constant_time_count(r), p, c);
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

template<typename Op, typename Func, typename State>
ALWAYS_INLINE_HIDDEN reduce_op<Op, Func, State> make_reduce_op(Op op, Func func, State state) {
  return {cmove(op), cmove(func), cmove(state)};
}


template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_nonempty_impl(Range r, Op op, Func f) {
  assert(!is_empty(r));
  auto tmp = for_each_impl(successor(r), make_reduce_op(op, f, *get_begin(r)));
  return range2::make_pair(cmove(tmp.m0.state), cmove(tmp.m1));
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonempty(Range r, Op op, Func f) -> decltype( reduce_nonempty_impl(add_constant_time_count(r), op, f) ) {
  assert(!is_empty(r));
  return reduce_nonempty_impl(add_constant_time_count(r), op, f);
}


template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_impl(Range r, Op op, Func f, RangeValue<Range> const& z) {
  if (is_empty(r)) return range2::make_pair(z, r);
  return reduce_nonempty_impl(add_constant_time_count(r), op, f);
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_impl(add_constant_time_count(r), op, f, z) ) {
  return reduce_impl(add_constant_time_count(r), op, f, z);
}

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

template<typename Op, typename Func, typename State>
ALWAYS_INLINE_HIDDEN reduce_nonzeroes_op<Op, Func, State> make_reduce_nonzeroes_op(Op op, Func func, State state, State const& z) {
  return {cmove(op), cmove(func), cmove(state), z};
}


template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_nonzeroes_impl(Range r, Op op, Func f, RangeValue<Range> const& z) {
  auto firstNonZero = find_if_impl(r, [&z, &f](RangeIterator<Range> x) -> bool { return z != f(x); });
  if (!is_empty(firstNonZero)) {
    auto tmp = for_each_impl(successor(firstNonZero), make_reduce_nonzeroes_op(op, f, f(get_begin(firstNonZero)), z));
    return range2::make_pair(tmp.m0.state, tmp.m1);
  } else {
    return range2::make_pair(z, r);
  }
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonzeroes(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z) ) {
  return reduce_nonzeroes_impl(add_constant_time_count(r), op, f, z);
}


template<typename Range0, typename Range1, typename Rel>
INLINE typename std::enable_if<!IsACountedRange<Range0>::value || !IsACountedRange<Range1>::value, pair<Range0, Range1>>::type
find_mismatch_impl(Range0 r0, Range1 r1, Rel rel) {
  while (!is_empty(r0) && !is_empty(r1) && rel(get_begin(r0), get_begin(r1))) {
    r0 = successor(r0), r1 = successor(r1);
  }
  return range2::make_pair(r0, r1);
}

template<typename Range0, typename Range1, typename Rel>
INLINE typename std::enable_if<IsACountedRange<Range0>::value && IsACountedRange<Range1>::value, pair<Range0, Range1>>::type
  find_mismatch_impl(Range0 r0, Range1 r1, Rel rel) {
  if (get_count(r1) > get_count(r0)) {
    auto diff = get_count(r1) - get_count(r0); // +ve
    auto tmp = find_mismatch_impl(r0, make_range(get_begin(r1), NotPresent{}, NotPresent{}), rel);
    return range2::make_pair(tmp.m0, make_range(get_begin(tmp.m1), get_end(r1), diff + get_count(tmp.m0)));
  } else {
    auto diff = get_count(r0) - get_count(r1); // 0 or +ve
    auto tmp = find_mismatch_impl(make_range(get_begin(r0), NotPresent{}, NotPresent{}), r1, rel);
    return range2::make_pair(make_range(get_begin(tmp.m0), get_end(r0), diff + get_count(tmp.m1)), tmp.m1);
  }
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_mismatch(Range0 r0, Range1 r1, Rel rel) -> decltype( find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel) ) {
  return find_mismatch_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel);
}


template<typename Range, typename Rel>
INLINE pair<RangeValue<Range>, Range> find_adjacent_mismatch_input_non_empty_impl(Range r, Rel rel) {
  assert(!is_empty(r));
  RangeValue<Range> tmp = *get_begin(r);
  r = successor(r);
  while (!is_empty(r)) {
    if (!rel(&tmp, get_begin(r))) return range2::make_pair(tmp, r);
    tmp = *get_begin(r);
    r = successor(r);
  }
  return range2::make_pair(tmp, r);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch_input_non_empty(Range r, Rel rel) -> decltype( find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel) ) {
  return find_adjacent_mismatch_input_non_empty_impl(add_constant_time_count(r), rel);
}


template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN Range
find_adjacent_mismatch_impl(Range r, Rel rel) {
  if(is_empty(r)) return r;
  auto prev = r; // Initialise prev to something, anything.
  do {
    prev = r;
    r = successor(r);
  } while (!is_empty(r) && rel(get_begin(prev), get_begin(r)));
  return r;
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto find_adjacent_mismatch(Range r, Rel rel) -> decltype( find_adjacent_mismatch_impl(add_constant_time_count(r), rel) ) {
  return find_adjacent_mismatch_impl(add_constant_time_count(r), rel);
}


template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool relation_preserving_impl(Range r, Rel rel, std::input_iterator_tag) {
  if (is_empty(r)) return true;
  return is_empty(find_adjacent_mismatch_input_non_empty(r, rel).m1);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool relation_preserving_impl(Range r, Rel rel, std::forward_iterator_tag) {
  return is_empty(find_adjacent_mismatch_impl(r, rel));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool relation_preserving(Range r, Rel rel) {
  return relation_preserving_impl(add_constant_time_count(r), rel, get_category(r));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool strictly_increasing_range(Range r, Rel rel) {
  return relation_preserving(r, rel);
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool increasing_range(Range r, Rel rel) {
  return relation_preserving(r, make_complement_converse(rel));
}


template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool partitioned_impl(Range r, Rel rel) {
  return is_empty(find_if_impl(find_if_impl(r, rel), make_complement(rel)));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN bool partitioned(Range r, Rel rel) {
  return partitioned_impl(add_constant_time_count(r), rel);
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

template<typename Rng, RangeDifferenceType<Rng> LinearSearchLimit, typename Pred, typename BisectionOperation>
// requires BisectionOperation return a value < the value passed in, and is also >= 0.
ALWAYS_INLINE_HIDDEN pair<Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, typename GetEnd<Rng>::type, Present>> bisecting_search(Rng r, Pred pred, BisectionOperation bo) {
  auto iter = get_begin(r);
  auto n = get_count(r);
  auto lhsN = decltype(n)(0);

  while (decltype(n){0} != n) {
    if (n <= LinearSearchLimit) {
      auto tmp = find_if_impl(make_range(iter, NotPresent{}, n), pred);
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

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto partition_point_impl(Range r, Pred pred) -> decltype( bisecting_search<Range, 0>(r, pred, impl::Halve{}) ) {
  return bisecting_search<Range, 0>(r, pred, impl::Halve{});
}

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto partition_point(Range r, Pred pred) -> decltype( partition_point_impl(add_linear_time_count(r), pred) ) {
  static_assert(IsAFiniteRange<Range>::value, "Must be a finite range to perform binary search");
  return partition_point_impl(add_linear_time_count(r), pred);
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
ALWAYS_INLINE_HIDDEN lower_bound_pred<Value, Rel> make_lower_bound_pred(Value const& v, Rel rel) {
  return {v, cmove(rel)};
}


template<typename Value, typename Rel>
struct TYPE_HIDDEN_VISIBILITY upper_bound_pred {
  Value const& v;
  Rel rel;

  bool ALWAYS_INLINE_HIDDEN operator()(Value const& x) {
    return rel(v, x);
  }
};

template<typename Value, typename Rel>
ALWAYS_INLINE_HIDDEN upper_bound_pred<Value, Rel> make_upper_bound_pred(Value const& v, Rel rel) {
  return {v, cmove(rel)};
}

} // namespace impl

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto lower_bound_predicate_impl(Range r, Rel rel, RangeValue<Range> const& a) -> decltype ( partition_point_impl(r, make_derefop(impl::lower_bound_pred<RangeValue<Range>, Rel>{a, rel})) ) {
  return partition_point_impl(r, make_derefop(impl::make_lower_bound_pred(a, rel)));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto lower_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( lower_bound_predicate_impl(add_linear_time_count(r), rel, a) ) {
  return lower_bound_predicate_impl(add_linear_time_count(r), rel, a);
}


template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto upper_bound_predicate_impl(Range r, Rel rel, RangeValue<Range> const& a) -> decltype ( partition_point_impl(r, make_derefop(impl::upper_bound_pred<RangeValue<Range>, Rel>{a, rel})) ){
  return partition_point_impl(r, make_derefop(impl::make_upper_bound_pred(a, rel)));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto upper_bound_predicate(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( upper_bound_predicate_impl(add_linear_time_count(r), rel, a) ) {
  return upper_bound_predicate_impl(add_linear_time_count(r), rel, a);
}

template<typename Rng, typename Rel, typename BisectionOperation>
ALWAYS_INLINE_HIDDEN triple<Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, Present, Present>, Range<RangeIterator<Rng>, typename GetEnd<Rng>::type, Present>>
equivalent_range_impl(Rng r, Rel rel, RangeValue<Rng> const& a, BisectionOperation bo) {
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
      auto lower_bound = lower_bound_predicate_impl(make_range(iter, NotPresent{}, n), rel, a);
      auto upper_bound = upper_bound_predicate_impl(lower_bound.m1, rel, a);

      return make_triple(make_range(get_begin(r), get_begin(lower_bound.m1), get_count(lower_bound.m0) + lhsN),
       upper_bound.m0,
       make_range(get_begin(upper_bound.m1), get_end(r), get_count(r) - get_count(upper_bound.m0) - get_count(lower_bound.m0) - lhsN));
    }
  }
  return make_triple(make_range(get_begin(r), iter, lhsN), make_range(iter, iter, 0), make_range(iter, get_end(r), get_count(r) - lhsN));
}

template<typename Range, typename Rel>
ALWAYS_INLINE_HIDDEN auto equivalent_range(Range r, Rel rel, RangeValue<Range> const& a) -> decltype( equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{}) ) {
  return equivalent_range_impl(add_linear_time_count(r), rel, a, impl::Halve{});
}



template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_equivalent_impl2(Range0 r0, Range1 r1, Rel rel) {
  auto tmp = find_mismatch_impl(r0, r1, rel);
  return is_empty(tmp.m0) && is_empty(tmp.m1);
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN typename std::enable_if<IsACountedRange<Range0>::value && IsACountedRange<Range1>::value, bool>::type
lexicographical_equivalent_impl(Range0 r0, Range1 r1, Rel rel) {
  if (get_count(r0) != get_count(r1)) {
    return false ;
  } else {
    return lexicographical_equivalent_impl2(r0, r1, rel);
  }
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN typename std::enable_if<!IsACountedRange<Range0>::value || !IsACountedRange<Range1>::value, bool>::type
lexicographical_equivalent_impl(Range0 r0, Range1 r1, Rel rel) {
  return lexicographical_equivalent_impl2(r0, r1, rel);
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_equivalent(Range0 r0, Range1 r1, Rel rel) {
  return lexicographical_equivalent_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel);
}


template<typename Range0, typename Range1>
ALWAYS_INLINE_HIDDEN bool lexicographical_equal(Range0 r0, Range1 r1) {
  static_assert(std::is_same<RangeValue<Range0>, RangeValue<Range1>>::value, "Both ranges must have the same value type");
  return lexicographical_equivalent(r0, r1, make_derefop(std::equal_to<RangeValue<Range0>>{}));
}


template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_compare_impl(Range0 r0, Range1 r1, Rel rel) {
  auto tmp = find_mismatch_impl(r0, r1, make_derefop(make_equivalent(rel)));
  if (is_empty(tmp.m1)) {
    return false;
  } else if (is_empty(tmp.m0)) {
    return true;
  } else {
    // Therefore *get_begin(tmp.m0) is not equivalent to *get_begin(tmp.m1).
    return rel(*get_begin(tmp.m0), *get_begin(tmp.m1));
  }
}

template<typename Range0, typename Range1, typename Rel>
ALWAYS_INLINE_HIDDEN bool lexicographical_compare(Range0 r0, Range1 r1, Rel rel) {
  return lexicographical_compare_impl(add_constant_time_count(r0), add_constant_time_count(r1), rel);
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
    i = successor(i), o = successor(o);
  }
};

struct TYPE_DEFAULT_VISIBILITY move_step
{
  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) const {
    sink(get_begin(o), std::move(*get_begin(i)));
    i = successor(i), o = successor(o);
  }
};

struct TYPE_DEFAULT_VISIBILITY swap_step
{
  template<typename I, typename O>
  ALWAYS_INLINE_HIDDEN void operator()(I& i, O& o) const {
    using std::swap;
    swap(*get_begin(i), *get_begin(o));
    i = successor(i), o = successor(o);
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
      i = successor(i);
    }
  }
};

template<typename Pred, typename Step>
ALWAYS_INLINE_HIDDEN step_if<Pred, Step> make_step_if(Pred p, Step s) {
  return {cmove(p), cmove(s)};
}


template<typename R0, typename R1, typename Step>
INLINE pair<R0, R1>
visit_2_ranges_impl(R0 r0, R1 r1, Step step) {
  while (!is_empty(r0) && !is_empty(r1)) step(r0, r1);
  return make_pair(r0, r1);
}

template<typename R0, typename R1, typename Step>
ALWAYS_INLINE_HIDDEN auto visit_2_ranges(R0 r0, R1 r1, Step step) -> decltype ( visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value, "One of the ranges must be finite");
  return visit_2_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), step);
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
  return {cmove(p), cmove(s)};
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
  return {cmove(r), cmove(s)};
}

template<typename R0, typename R1, typename R2, typename Step>
INLINE triple<R0, R1, R2>
visit_3_ranges_impl(R0 r0, R1 r1, R2 r2, Step step) {
  while (!is_empty(r0) && !is_empty(r1) && !is_empty(r2)) step(r0, r1, r2);
  return make_triple(r0, r1, r2);
}

template<typename R0, typename R1, typename R2, typename Step>
ALWAYS_INLINE_HIDDEN auto visit_3_ranges(R0 r0, R1 r1, R2 r2, Step step) -> decltype ( visit_3_ranges_impl (add_constant_time_count(r0),add_constant_time_count(r1), add_constant_time_count(r2), step) ) {
  static_assert(IsAFiniteRange<R0>::value || IsAFiniteRange<R1>::value || IsAFiniteRange<R2>::value, "One of the ranges must be finite");
  return visit_3_ranges_impl(add_constant_time_count(r0), add_constant_time_count(r1), add_constant_time_count(r2), step);
}

} // namespace range2

#endif
