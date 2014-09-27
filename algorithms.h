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

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE Op
for_each_impl(Range r, Op op, NoInline) {
  while (!is_empty(r)) {
    op(get_begin(r));
    r = next(r);
  }
  return op;
}

template<typename Range, typename Op, typename InliningPreferences>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE typename std::enable_if<!IsInlineable<Range>::value, Op>::type
for_each_impl(Range r, Op op, InliningPreferences) {
  return for_each_impl(r, op, NoInline{});
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE typename std::enable_if<IsInlineable<Range>::value, Op>::type
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
  return op;
}


template<typename Range, typename Op, typename InliningPreferences>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE Op for_each(Range r, Op op, InliningPreferences p) {
  static_assert(IsABoundedRange<Range>::value, "Must be a bounded range");

  return for_each_impl(add_constant_time_end(r), op, p);
}

template<typename Range, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range)
INLINE Op for_each(Range r, Op op) {
  static_assert(IsABoundedRange<Range>::value, "Must be a bounded range");

  return for_each_impl(add_constant_time_end(r), op, NoInline{});
}



template<typename Range0, typename Range1, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range0) &&
// input_type(Op, 1) == RangeIterator(Range1)
INLINE triple<Range0, Range1, Op> for_each2_impl(Range0 r0, Range1 r1, Op op) {

  while(!is_empty(r0) && !is_empty(r1)) {
    op(get_begin(r0), get_begin(r1));
    r0 = next(r0), r1 = next(r1);
  }
  return make_triple(cmove(r0), cmove(r1), cmove(op));
}

template<typename Range0, typename Range1, typename Op>
// Requires input_type(Op, 0) == RangeIterator(Range0) &&
// input_type(Op, 1) == RangeIterator(Range1)
ALWAYS_INLINE_HIDDEN auto for_each2(Range0 r0, Range1 r1, Op op) -> decltype( for_each2_impl(add_constant_time_end(r0), add_constant_time_end(r1), op) ) {
  static_assert(IsABoundedRange<Range0>::value || IsABoundedRange<Range1>::value, "One of the input ranges must be a bounded range");

  return for_each2_impl(add_constant_time_end(r0), add_constant_time_end(r1), op);
}



template<typename Range, typename Pred, typename InliningPreferences>
INLINE typename std::enable_if<IsACountedRange<Range>::value, Range>::type
find_if(Range r, Pred p, InliningPreferences x) {
  auto count = get_count(x);
  auto begin = get_begin(x);
  while (count && !p(begin)) {
    --count, ++begin;
  }
  return make_range(begin, get_end(r), count);
}

template<typename Range, typename Pred, typename InliningPreferences>
INLINE typename std::enable_if<!IsACountedRange<Range>::value, Range>::type
find_if(Range r, Pred p, InliningPreferences x) {
  while (!is_empty(r) && !p(get_begin(r))) r = next(r);
  return r;
}

template<typename Range, typename Pred, typename InliningPreferences>
INLINE auto find_if(Range r, Pred p, InliningPreferences x) -> decltype ( find_if_impl(add_constant_time_end(r), p, x) ) {
  return find_if_impl(add_constant_time_end(r), p, x);
}

template<typename Range, typename Pred>
INLINE auto find_if(Range r, Pred p) -> decltype ( find_if_impl(add_constant_time_end(r), p, NoInline{}) ) {
  return find_if_impl(add_constant_time_end(r), p, NoInline{});
}



template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
INLINE CountType count_if_impl(Range r, Pred p, CountType c, InliningPreferences x) {
  for_each(r, [&p, &c](RangeIterator<Range> const& x) -> void { if(p(*x)) ++c; }, x);
  return c;
}

template<typename Range, typename Pred, typename CountType, typename InliningPreferences>
INLINE CountType count_if(Range r, Pred p, CountType c, InliningPreferences x) {
  return count_if_impl(add_constant_time_end(r), p, c, x);
}

template<typename Range, typename Pred, typename CountType>
INLINE CountType count_if(Range r, Pred p, CountType c) {
  return count_if_impl(add_constant_time_end(r), p, c, NoInline{});
}

} // namespace range2

#endif
