#ifndef INCLUDED_ALGORITHMS
#define INCLUDED_ALGORITHMS

#ifndef INCLUDED_RANGE2
#include "range2.h"
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

  template<typename Iterator>
  // Requires input_type(Op, 0) == ValueType<Iterator>
  ALWAYS_INLINE_HIDDEN auto operator()(Iterator x) -> decltype( op(*x) ) {
    return op(*x);
  }
};

template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY Deref2Op
{
  Op op;

  template<typename Iterator0, typename Iterator1>
  // Requires input_type(Op, 0) == ValueType<Iterator0> &&
  // input_type(Op, 1) == ValueType<Iterator1>
  ALWAYS_INLINE_HIDDEN auto operator()(Iterator0 x, Iterator1 y) -> decltype( op(*x, *y) ) {
    return op(*x, *y);
  }
};

template<typename Op>
// Requires BinaryOperation(Op)
struct TYPE_HIDDEN_VISIBILITY complement
{
  Op op;

  template<typename T0, typename T1>
  // Requires input_type(Op, 0) == T0
  // input_type(Op, 1) == T1
  ALWAYS_INLINE_HIDDEN auto operator()(T0&& x, T1&& y) -> decltype( !op(std::forward<T0>(x), std::forward<T1>(y)) ) {
    return !op(std::forward<T0>(x), std::forward<T1>(y));
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

  while (countBy4) {
    --countBy4;
    auto iter1 = successor(iter);
    auto iter2 = successor(iter1);
    auto iter3 = successor(iter2);
    op(iter);
    iter = successor(iter3);
    op(iter1);
    op(iter2);
    op(iter3);
  }

  while (count) {
    --count;
    op(iter);
    iter = successor(iter);
  }
  return range2::make_pair(op, make_range(iter, get_end(r), 0));
}

template<typename Range, typename Op, typename InliningPreferences>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN auto for_each(Range r, Op op, InliningPreferences p) -> decltype( for_each_impl(add_constant_time_end(r), op, p) ) {
  static_assert(IsABoundedRange<Range>::value, "Must be a bounded range");

  return for_each_impl(add_constant_time_end(r), op, p);
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
ALWAYS_INLINE_HIDDEN auto for_each(Range r, Op op) -> decltype( for_each_impl(add_constant_time_end(r), op, NoInline{}) ) {
  static_assert(IsABoundedRange<Range>::value, "Must be a bounded range");

  return for_each_impl(add_constant_time_end(r), op, NoInline{});
}



template<typename Range, typename Pred, typename InliningPreferences>
INLINE typename std::enable_if<IsACountedRange<Range>::value, Range>::type
find_if_impl(Range r, Pred p, InliningPreferences x) {
  auto count = get_count(r);
  auto begin = get_begin(r);
  while (count && !p(begin)) {
    --count, ++begin;
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
ALWAYS_INLINE_HIDDEN auto find_if(Range r, Pred p, InliningPreferences x) -> decltype ( find_if_impl(add_constant_time_end(r), p, x) ) {
  return find_if_impl(add_constant_time_end(r), p, x);
}

template<typename Range, typename Pred>
ALWAYS_INLINE_HIDDEN auto find_if(Range r, Pred p) -> decltype ( find_if_impl(add_constant_time_end(r), p, NoInline{}) ) {
  return find_if_impl(add_constant_time_end(r), p, NoInline{});
}



template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN CountType count_if_impl(Range r, Pred p, CountType c, InliningPreferences x) {
  for_each_impl(r, [&p, &c](RangeIterator<Range> const& x) -> void { if(p(x)) ++c; }, x);
  return c;
}

template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN CountType count_if(Range r, Pred p, CountType c, InliningPreferences x) {
  return count_if_impl(add_constant_time_end(r), p, c, x);
}

template<typename Range, typename Pred, typename CountType>
ALWAYS_INLINE_HIDDEN CountType count_if(Range r, Pred p, CountType c) {
  return count_if_impl(add_constant_time_end(r), p, c, NoInline{});
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
  // requires !is_empty(r)
  auto operation = reduce_op<Op, Func, RangeValue<Range>>{op, f, *get_begin(r)};
  auto tmp = for_each_impl(next(r), operation, p);
  return range2::make_pair(cmove(tmp.m0.state), cmove(tmp.m1));
}

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto reduce_nonempty(Range r, Op op, Func f, InliningPreferences p) -> decltype( reduce_nonempty_impl(add_constant_time_end(r), op, f, p) ) {
  // requires !is_empty(r)
  return reduce_nonempty_impl(add_constant_time_end(r), op, f, p);
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonempty(Range r, Op op, Func f) -> decltype( reduce_nonempty_impl(add_constant_time_end(r), op, f, NoInline{}) ) {
  // requires !is_empty(r)
  return reduce_nonempty_impl(add_constant_time_end(r), op, f, NoInline{});
}


template<typename Range, typename Op, typename Func, typename InlingPreferences>
ALWAYS_INLINE_HIDDEN pair<RangeValue<Range>, Range> reduce_impl(Range r, Op op, Func f, RangeValue<Range> const& z, InlingPreferences p) {
  if (is_empty(r)) return range2::make_pair(z, r);
  return reduce_nonempty_impl(add_constant_time_end(r), op, f, p);
}

template<typename Range, typename Op, typename Func, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto reduce(Range r, Op op, Func f, RangeValue<Range> const& z, InliningPreferences p) -> decltype( reduce_impl(add_constant_time_end(r), op, f, z, p) ) {
  return reduce_impl(add_constant_time_end(r), op, f, z, p);
}

template<typename Range, typename Op, typename Func>
  ALWAYS_INLINE_HIDDEN auto reduce(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_impl(add_constant_time_end(r), op, f, z, NoInline{}) ) {
  return reduce_impl(add_constant_time_end(r), op, f, z, NoInline{});
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
ALWAYS_INLINE_HIDDEN auto reduce_nonzeroes(Range r, Op op, Func f, RangeValue<Range> const& z, InliningPreferences p) -> decltype( reduce_nonzeroes_impl(add_constant_time_end(r), op, f, z, p) ) {
  return reduce_nonzeroes_impl(add_constant_time_end(r), op, f, z, p);
}

template<typename Range, typename Op, typename Func>
ALWAYS_INLINE_HIDDEN auto reduce_nonzeroes(Range r, Op op, Func f, RangeValue<Range> const& z) -> decltype( reduce_nonzeroes_impl(add_constant_time_end(r), op, f, z, NoInline{}) ) {
  return reduce_nonzeroes_impl(add_constant_time_end(r), op, f, z, NoInline{});
}


template<typename Range0, typename Range1, typename Op, typename InliningPreferences>
INLINE pair<Range0, Range1> find_mismatch_impl(Range0 r0, Range1 r1, Op op, InliningPreferences p) {

  // TODO if both are counted ranges just check on the minimum of get_count(r0) and get_count(r1)
  while(!is_empty(r0) && !is_empty(r1) && !op(get_begin(r0), get_begin(r1))) {
    r0 = next(r0), r1 = next(r1);
  }
  return range2::make_pair(r0, r1);
}

template<typename Range0, typename Range1, typename Op, typename InliningPreferences>
ALWAYS_INLINE_HIDDEN auto find_mismatch(Range0 r0, Range1 r1, Op op, InliningPreferences p) -> decltype( find_mismatch_impl(add_constant_time_end(r0), add_constant_time_end(r1), op, NoInline{}) ) {
  return find_adjacent_mismatch_impl(add_constant_time_end(r0), add_constant_time_end(r1), op, p);
}

template<typename Range0, typename Range1, typename Op>
ALWAYS_INLINE_HIDDEN auto find_mismatch(Range0 r0, Range1 r1, Op op) -> decltype( find_mismatch_impl(add_constant_time_end(r0), add_constant_time_end(r1), op, NoInline{}) ) {
  return find_adjacent_mismatch_impl(add_constant_time_end(r0), add_constant_time_end(r1), op, NoInline{});
}

} // namespace range2

#endif
