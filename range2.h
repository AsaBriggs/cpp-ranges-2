#ifndef INCLUDED_RANGE2
#define INCLUDED_RANGE2

#ifndef INCLUDED_TYPE_TRAITS
#define INCLUDED_TYPE_TRAITS
#include <type_traits>
#endif

#ifndef INCLUDED_ITERATOR
#define INCLUDED_ITERATOR
#include <iterator>
#endif

#ifndef INCLUDED_COMPILER_SPECIFICS
#include "compiler_specifics.h"
#endif

#ifndef INCLUDED_ITERATOR_ADAPTER
#include "iterator_adapter.h"
#endif

#ifndef INCLUDED_COMPARISONS
#include "comparisons.h"
#endif

namespace range2 {

struct TYPE_HIDDEN_VISIBILITY ConstantComplexity { typedef ConstantComplexity type; };
struct TYPE_HIDDEN_VISIBILITY LogarithmicComplexity { typedef LogarithmicComplexity type; };
struct TYPE_HIDDEN_VISIBILITY LinearComplexity { typedef LinearComplexity type; };
struct TYPE_HIDDEN_VISIBILITY LogarithmicLinearComplexity { typedef LogarithmicLinearComplexity type; };
struct TYPE_HIDDEN_VISIBILITY UndefinedComplexity { typedef UndefinedComplexity type; };


template<bool p, typename T, typename F>
struct TYPE_HIDDEN_VISIBILITY if_;

template<typename T, typename F>
struct TYPE_HIDDEN_VISIBILITY if_<true, T, F> {
  typedef T type;
};

template<typename T, typename F>
struct TYPE_HIDDEN_VISIBILITY if_<false, T, F> {
  typedef F type;
};


template<typename Op, typename... Params>
struct TYPE_HIDDEN_VISIBILITY Complexity;

struct TYPE_HIDDEN_VISIBILITY Advance { typedef Advance type; };
struct TYPE_HIDDEN_VISIBILITY Distance { typedef Distance type; };

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY Complexity<Advance, Iterator> : if_<std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag>::value, ConstantComplexity, LinearComplexity> {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY Complexity<Distance, Iterator> : if_<std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag>::value, ConstantComplexity, LinearComplexity> {};

// cmove = constexpr move ... in C++11 std::move is not a constexpr!
template<typename T>
constexpr ALWAYS_INLINE_HIDDEN
typename std::remove_reference<T>::type&& cmove(T&& t) {
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}

template<typename T0, typename T1>
struct TYPE_DEFAULT_VISIBILITY pair
{
    typedef pair type;
    T0 m0;
    T1 m1;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type const& x, type const& y) { return (x.m0 == y.m0) && (x.m1 == y.m1); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return (x.m0 < y.m0) || (!(y.m0 < x.m0) && (x.m1 < y.m1)); }
};

template<typename T0, typename T1>
struct GenerateDerivedComparisonOperations<pair<T0, T1>> : std::true_type {};

template<typename T0, typename T1>
constexpr ALWAYS_INLINE_HIDDEN pair<T0, T1> make_pair(T0 m0, T1 m1) {
  return {cmove(m0), cmove(m1)};
}

template<typename T0, typename T1, typename T2>
struct TYPE_DEFAULT_VISIBILITY triple
{
    typedef triple type;
    T0 m0;
    T1 m1;
    T2 m2;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type const& x, type const& y) { return (x.m0 == y.m0) && (x.m1 == y.m1) && (x.m2 == y.m2); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return (x.m0 < y.m0) || (!(y.m0 < x.m0) && ((x.m1 < y.m1) || (!(y.m1 < x.m1) && (x.m2 < y.m2)))); }
};

template<typename T0, typename T1, typename T2>
struct GenerateDerivedComparisonOperations<triple<T0, T1, T2>> : std::true_type {};

template<typename T0, typename T1, typename T2>
constexpr ALWAYS_INLINE_HIDDEN triple<T0, T1, T2> make_triple(T0 m0, T1 m1, T2 m2) {
  return {cmove(m0), cmove(m1), cmove(m2)};
}

struct TYPE_DEFAULT_VISIBILITY NotPresent {
    typedef NotPresent type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return false; }
};

template<>
struct GenerateDerivedComparisonOperations<NotPresent> : std::true_type {};

struct TYPE_DEFAULT_VISIBILITY Present {
    typedef Present type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return false; }
};

template<>
struct GenerateDerivedComparisonOperations<Present> : std::true_type {};

template<typename Iterator, typename End=NotPresent, typename Count=NotPresent>
struct TYPE_DEFAULT_VISIBILITY Range;

template<typename T>
struct TYPE_HIDDEN_VISIBILITY IsABoundedRange_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY IsABoundedRange_Impl <Range<Iterator, End, Count>> : std::integral_constant<bool, !std::is_same<End, NotPresent>::value || ! std::is_same<Count, NotPresent>::value> {};

template<typename T>
using IsABoundedRange = IsABoundedRange_Impl<T>;


template<typename T>
struct TYPE_HIDDEN_VISIBILITY IsACountedRange_Impl : std::false_type {};

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY IsACountedRange_Impl<Range<Iterator, End, Present>> : std::true_type {};

template<typename T>
using IsACountedRange = IsACountedRange_Impl<T>;


template<typename T>
struct TYPE_HIDDEN_VISIBILITY RangeIterator_Impl;

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY RangeIterator_Impl<Range<Iterator, End, Count>> {
  typedef Iterator type;
};

template<typename T>
using RangeIterator = typename RangeIterator_Impl<T>::type;

template<typename T>
using RangeIteratorTraits = std::iterator_traits<RangeIterator<T>>;

template<typename T>
using RangeDifferenceType = typename RangeIteratorTraits<T>::difference_type;

template<typename T>
using RangeIteratorCategory = typename std::iterator_traits<RangeIterator<T>>::iterator_category;


template<typename T>
struct TYPE_HIDDEN_VISIBILITY IsInlineable : std::integral_constant<bool, IsACountedRange<T>::value && std::is_convertible<RangeIteratorCategory<T>, std::forward_iterator_tag>::value> {};


template<typename Iterator, typename End, typename Count>
struct TYPE_DEFAULT_VISIBILITY Range
{
    typedef Range type;
    Iterator begin;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, NotPresent, NotPresent) {
        return {cmove(begin)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return false; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return x.begin == y.begin; }
};

template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, Iterator end, NotPresent) {
        return {cmove(begin), cmove(end)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return x.begin == x.end; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end); }
};


template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeDifferenceType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, Iterator end, RangeDifferenceType<type> count) {
        return {cmove(begin), cmove(end), cmove(count)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return RangeDifferenceType<type>(0) == x.count; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }
};

template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present>
{
    typedef Range type;
    Iterator begin;
    RangeDifferenceType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, NotPresent, RangeDifferenceType<type> count) {
        return {cmove(begin), cmove(count)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return (RangeDifferenceType<type>(0) == x.count); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }
};


template<typename T>
struct TYPE_HIDDEN_VISIBILITY GetPresence : Present {};

template<>
struct TYPE_HIDDEN_VISIBILITY GetPresence<NotPresent> : NotPresent {};

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type>
make_range(Iterator begin, End end, Count count) {
  static_assert(std::is_same<End, NotPresent>::value || std::is_convertible<End, Iterator>::value, "End Iterator must be NotPresent or of the same type as begin");
  static_assert(std::is_same<Count, NotPresent>::value || std::is_convertible<DifferenceType<Iterator>, Count>::value, "Count must NotPresent or of type DifferenceType<Iterator>");

  return Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type>::make(cmove(begin), cmove(end), cmove(count));
}


template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator&
get_begin(Range<Iterator, End, Count>& x) {
    return x.begin;
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator const&
get_begin(Range<Iterator, End, Count> const& x) {
    return x.begin;
}


template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
get_end(Range<Iterator, NotPresent, Count> const& x) {
  return {};
}

template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator&
get_end(Range<Iterator, Present, Count>& x) {
    return x.end;
}

template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator const&
get_end(Range<Iterator, Present, Count> const& x) {
    return x.end;
}


template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
get_count(Range<Iterator, End, NotPresent> const& x) {
  return {};
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN DifferenceType<Iterator>&
get_count(Range<Iterator, End, Present>& x) {
  return x.count;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN DifferenceType<Iterator> const&
get_count(Range<Iterator, End, Present> const& x) {
  return x.count;
}


template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN
bool operator< (Range<Iterator, End, Count> const& x, Range<Iterator, End, Count> const& y) {
  return (get_begin(x) < get_begin(y)) ||
         (!(get_begin(y) < get_begin(x)) && ((get_end(x) < get_end(y) ||
					    (!(get_end(y) < get_end(x)) && (get_count(x) < get_count(y))))));
}

template<typename Iterator, typename End, typename Count>
struct GenerateDerivedComparisonOperations<Range<Iterator, End, Count>> : std::true_type {};


template<typename Iterator, typename End, typename Count, typename Iterator2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count>
add_begin(Range<Iterator, End, Count> const& x, Iterator2 begin) {
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "Begin iterator must be convertible to Range's Iterator");
  return Range<Iterator, End, Count>::make(begin, get_end(x), get_count(x));
}

template<typename Iterator, typename End, typename Count, typename Iterator2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
add_end(Range<Iterator, End, Count> const& x, Iterator2 end) {
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "End iterator must be convertible to Range's Iterator");
  return Range<Iterator, Present, Count>::make(get_begin(x), cmove(end), get_count(x));
}

template<typename Iterator, typename End, typename Count, typename Count2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present>
add_count(Range<Iterator, End, Count> const& x, Count2 count) {
  static_assert(std::is_convertible<Count2, DifferenceType<Iterator>>::value, "Count must be convertible to Range's DifferenceType");
  return Range<Iterator, End, Present>::make(get_begin(x), get_end(x), count);
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, NotPresent, Count>
remove_end(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, NotPresent, Count>::make(get_begin(x), {}, get_count(x));
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, NotPresent>
remove_count(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, End, NotPresent>::make(get_begin(x), get_end(x), {});
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN bool
is_empty(Range<Iterator, End, Count> const& x) {
  // ADL
  return empty_impl(x);
}

namespace impl {

constexpr ALWAYS_INLINE_HIDDEN NotPresent decrementCount(NotPresent) { return {}; }

template<typename Count>
constexpr ALWAYS_INLINE_HIDDEN Count decrementCount(Count x) { return x - 1; }

} // namespace impl

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count>
next(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, End, Count>::make(range2::advance(get_begin(x), 1), get_end(x), impl::decrementCount(get_count(x)));
}

template<typename T>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd : std::false_type {};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, Present, Count>> : std::true_type {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, NotPresent, Present>> : std::is_same<ConstantComplexity, typename Complexity<Advance, Iterator>::type> {};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY CanAddEndInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and a count is present to terminate the loop.
template<typename Iterator, typename Count>
struct CanAddEndInLinearTime<Range<Iterator, NotPresent, Count>> :
  std::integral_constant<bool,
   !ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value &&
   std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
   !std::is_same<Count, NotPresent>::value>
{};

struct TYPE_HIDDEN_VISIBILITY AddEnd { typedef AddEnd type; };

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddEnd, Range<Iterator, Present, Count>> : ConstantComplexity {};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddEnd, Range<Iterator, NotPresent, Count>, typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddEnd, Range<Iterator, NotPresent, Count>, typename std::enable_if<!ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, void>::type> : if_< CanAddEndInLinearTime<Range<Iterator, NotPresent, Count>>::value, LinearComplexity, UndefinedComplexity> {};



// Already present
template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
add_constant_time_end(Range<Iterator, Present, Count> const& x) {
  return x;
}

// Can add in constant time
template<typename Iterator, typename Count>
constexpr typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Count>>::type
add_constant_time_end(Range<Iterator, NotPresent, Count> const& x) {
  // Advance for randon access iterators is equivalent to pointer addition.
  return make_range(get_begin(x), range2::advance(get_begin(x), get_count(x)), get_count(x));
}

// Can't add in constant time
template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, NotPresent, Count>>::type
add_constant_time_end(Range<Iterator, NotPresent, Count> const& x) {
  return x;
}


template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
add_linear_time_end(Range<Iterator, Present, Count> const& x) {
  return x;
}

template<typename Iterator, typename Count>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Count>>::type
add_linear_time_end(Range<Iterator, NotPresent, Count> const& x) {
  return add_constant_time_end(x);
}

template<typename Iterator, typename Count>
typename std::enable_if<CanAddEndInLinearTime<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Present>>::type
add_linear_time_end(Range<Iterator, NotPresent, Count> const& x) {
  auto tmp = x;
  DifferenceType<Iterator> count = 0;
  while(!is_empty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return add_count(add_end(cmove(x), get_begin(tmp)), cmove(count));
}




template<typename T>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount : std::false_type {};

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount<Range<Iterator, End, Present>> : std::true_type {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount<Range<Iterator, Present, NotPresent>> : std::is_same<ConstantComplexity, typename Complexity<Distance, Iterator>::type> {};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY CanAddCountInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and a count is present to terminate the loop.
template<typename Iterator, typename End>
struct CanAddCountInLinearTime<Range<Iterator, End, NotPresent>> :
  std::integral_constant<bool,
  !ConstantTimeCount<Range<Iterator, End, NotPresent>>::value &&
  std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
  !std::is_same<End, NotPresent>::value>
{};

struct TYPE_HIDDEN_VISIBILITY AddCount { typedef AddCount type; };

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddCount, Range<Iterator, End, Present>> : ConstantComplexity {};

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddCount, Range<Iterator, End, Present>, typename std::enable_if<ConstantTimeCount<Range<Iterator, End, Present>>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY Complexity<AddCount, Range<Iterator, End, Present>, typename std::enable_if<!ConstantTimeCount<Range<Iterator, End, Present>>::value, void>::type> : if_<CanAddCountInLinearTime<Range<Iterator, End, Present>>::value, LinearComplexity, UndefinedComplexity> {};


// Already present
template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present>
add_constant_time_count(Range<Iterator, End, Present> const& x) {
  return x;
}

// Can add in constant time
template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, Present>>::type
add_constant_time_count(Range<Iterator, End, NotPresent> const& x) {
  return make_range(get_begin(x), get_end(x), std::distance(get_begin(x), get_end(x)));
}

// Can't add in constant time
template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, NotPresent>>::type
add_constant_time_count(Range<Iterator, End, NotPresent> const& x) {
  return x;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present>
add_linear_time_count(Range<Iterator, End, Present> const& x) {
  return x;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, Present>>::type
add_linear_time_count(Range<Iterator, End, NotPresent> const& x) {
  return add_constant_time_count(x);
}

template<typename Iterator, typename End>
ALWAYS_INLINE_HIDDEN typename std::enable_if<CanAddCountInLinearTime<Range<Iterator, End, NotPresent>>::value, Range<Iterator, Present, Present>>::type
add_linear_time_count(Range<Iterator, End, NotPresent> const& x) {
  auto tmp = x;
  DifferenceType<Iterator> count = 0;
  while(!is_empty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return make_range(get_begin(x), get_end(x), count);
}

struct TYPE_HIDDEN_VISIBILITY split_impl { typedef split_impl type; };

namespace impl {

template<typename Range, typename Middle, typename LHSCount, typename Enable=void>
struct TYPE_HIDDEN_VISIBILITY split_impl;

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, End, Count>, NotPresent, NotPresent>;

template<typename Iterator, typename End, typename Middle>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, End, NotPresent>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static constexpr ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(get_begin(x), middle, NotPresent{}), make_range(middle, get_end(x), NotPresent{})) ) {
    return make_pair(make_range(get_begin(x), middle, NotPresent{}), make_range(middle, get_end(x), NotPresent{}));
  }
};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, NotPresent, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, NotPresent, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(get_begin(x), middle, DifferenceType<Iterator>()), make_range(middle, NotPresent{}, DifferenceType<Iterator>()))) {
    // std::distance not constexpr in C++11
    DifferenceType<Iterator> diff = std::distance(get_begin(x), middle);
    DifferenceType<Iterator> c = get_count(x) - diff;
    return make_pair(make_range(get_begin(x), middle, diff), make_range(middle, NotPresent{}, c));
  }
};


template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, Present, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static constexpr ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, Present, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(get_begin(x), middle, NotPresent{}), make_range(middle, get_end(x), NotPresent{})) ) {
     // No need to calculate the diff as the end iterator is present
    return make_pair(make_range(get_begin(x), middle, NotPresent{}), make_range(middle, get_end(x), NotPresent{}));
  }
};



template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, End, Present>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static constexpr ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, Present> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(make_range(get_begin(x), middle, lhsCount), make_range(middle, get_end(x), get_count(x) - lhsCount)) ) {
    return make_pair(make_range(get_begin(x), middle, lhsCount), make_range(middle, get_end(x), get_count(x) - lhsCount));
  }
};


template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, End, NotPresent>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static constexpr ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(make_range(get_begin(x), middle, lhsCount), make_range(middle, get_end(x), NotPresent{})) ) {
    return make_pair(make_range(get_begin(x), middle, lhsCount), make_range(middle, get_end(x), NotPresent{}));
  }
};


template<typename Iterator, typename End, typename Count, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY split_impl<Range<Iterator, End, Count>, NotPresent, LHSCount, typename std::enable_if<!std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static constexpr ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, Count> const& x, NotPresent, LHSCount lhsCount) -> decltype( split_impl<Range<Iterator, End, Count>, Iterator, LHSCount>::apply(x, advance(get_begin(x), lhsCount), lhsCount) ) {
    // We must calculate the middle in order for the second range to be able to start somewhere.
    // Not really recursion as the split_impl type is different (Middle is now present).
    return split_impl<Range<Iterator, End, Count>, Iterator, LHSCount>::apply(x, advance(get_begin(x), lhsCount), lhsCount);
  } 
};

} // namespace impl

template<typename Iterator, typename End, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, End, NotPresent>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, NotPresent, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : Complexity<Distance, Iterator> {};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, Present, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, End, Present>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, End, NotPresent>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename End, typename Count, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<split_impl, Range<Iterator, End, Count>, NotPresent, LHSCount, typename std::enable_if<!std::is_same<NotPresent, LHSCount>::value, void>::type> : Complexity<Advance, Iterator> {};


// If input range is bounded then both returned ranges are bounded.
// Otherwise if input range in unbounded then first returned range is bounded and second returned range is unbounded.
template<typename Iterator, typename End, typename Count, typename Middle, typename LHSCount>
ALWAYS_INLINE_HIDDEN auto
split_at(Range<Iterator, End, Count> const& x, Middle middle, LHSCount lhsCount)
    -> decltype( impl::split_impl<Range<Iterator, End, Count>, Middle, LHSCount>::apply(x, cmove(middle), cmove(lhsCount))) {

  static_assert(std::is_same<Middle, NotPresent>::value || std::is_convertible<Middle, Iterator>::value, "Middle Iterator must be NotPresent or convertible to the same type as begin");
  static_assert(std::is_same<LHSCount, NotPresent>::value || std::is_convertible<DifferenceType<Iterator>, LHSCount>::value, "LHSCount must NotPresent or convertible to type DifferenceType<Iterator>");
  static_assert(!std::is_same<Middle, NotPresent>::value || !std::is_same<LHSCount, NotPresent>::value, "At least a middle or an lhs count must be supplied");

  return impl::split_impl<Range<Iterator, End, Count>, Middle, LHSCount>::apply(x, cmove(middle), cmove(lhsCount));
}

struct TYPE_HIDDEN_VISIBILITY Split_At { typedef Split_At type; };

template<typename Iterator, typename End, typename Count, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<Split_At, Range<Iterator, End, Count>, Middle, LHSCount> : Complexity<split_impl, Range<Iterator, End, Count>, Middle, LHSCount> {};

namespace impl {

ALWAYS_INLINE_HIDDEN NotPresent sumCount(NotPresent, NotPresent) { return {}; }

template<typename Count0>
ALWAYS_INLINE_HIDDEN NotPresent sumCount(Count0 const&, NotPresent) { return {}; }

template<typename Count1>
ALWAYS_INLINE_HIDDEN NotPresent sumCount(NotPresent, Count1 const&) { return {}; }

template<typename Count0, typename Count1>
ALWAYS_INLINE_HIDDEN Count0 sumCount(Count0 const& count0, Count1 const& count1) { return count0 + count1; }

} // namespace impl

// If the second range is bounded then the output range is bounded
template<typename Iterator0, typename End0, typename Count0, typename Iterator1, typename End1, typename Count1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, End1, Count1> const& y) -> decltype( make_range(get_begin(x), get_end(y), impl::sumCount(get_count(x), get_count(y))) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return make_range(get_begin(x), get_end(y), impl::sumCount(get_count(x), get_count(y)));
}

struct TYPE_HIDDEN_VISIBILITY Join { typedef Join type; };

template<typename Iterator0, typename End0, typename Count0, typename Iterator1, typename End1, typename Count1>
struct TYPE_HIDDEN_VISIBILITY Complexity<Join, Range<Iterator0, End0, Count0>, Range<Iterator1, End1, Count1>> : ConstantComplexity {};


template<typename Iterator0, typename End0, typename Count0, typename Iterator1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, NotPresent, Present> const& y) -> decltype( join(x, add_linear_time_end(y)) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return join(x, add_linear_time_end(y));
}

template<typename Iterator0, typename End0, typename Count0, typename Iterator1>
struct TYPE_HIDDEN_VISIBILITY Complexity<Join, Range<Iterator0, End0, Count0>, Range<Iterator1, NotPresent, Present>> : Complexity<AddEnd, Range<Iterator1, NotPresent, Present>> {};


struct TYPE_HIDDEN_VISIBILITY Reverse_Impl { typedef Reverse_Impl type; };

namespace impl {

template<typename T>
struct TYPE_HIDDEN_VISIBILITY reverse_impl;

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY reverse_impl<Range<Iterator, Present, Count>> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, Present, Count> const& x) -> decltype( make_range(make_reverse_iterator<Iterator>(get_end(x)), make_reverse_iterator<Iterator>(get_begin(x)), get_count(x)) ) {
    return make_range(make_reverse_iterator<Iterator>(get_end(x)), make_reverse_iterator<Iterator>(get_begin(x)), get_count(x));
  }
};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY reverse_impl<Range<Iterator, NotPresent, Present>> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, NotPresent, Present> const& x) -> decltype( reverse_impl<Range<Iterator, Present, Present>>::apply(add_linear_time_end(x)) ) {
    return reverse_impl<Range<Iterator, Present, Present>>::apply(add_linear_time_end(x));
  }
};

} // namespace impl

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse_Impl, Range<Iterator, Present, Count>> : ConstantComplexity {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse_Impl, Range<Iterator, NotPresent, Present>> : Complexity<AddEnd, Range<Iterator, NotPresent, Present>> {};


template<typename Iterator, typename End, typename Count>
ALWAYS_INLINE_HIDDEN auto
reverse(Range<Iterator, End, Count> const& x) -> decltype ( impl::reverse_impl<Range<Iterator, End, Count>>::apply(x) ) {

  static_assert(IsABoundedRange<Range<Iterator, End, Count>>::value, "Must be a bounded range");

  return impl::reverse_impl<Range<Iterator, End, Count>>::apply(x);
}

struct TYPE_HIDDEN_VISIBILITY Reverse { typedef Reverse type; };

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse, Range<Iterator, End, Count>> : Complexity<Reverse_Impl, Range<Iterator, End, Count>> {};

namespace impl {

template<SkipIteratorStride N>
ALWAYS_INLINE_HIDDEN NotPresent make_skip_iterator_impl(NotPresent x) {
  return x;
}

template<SkipIteratorStride N, typename Iterator>
ALWAYS_INLINE_HIDDEN auto make_skip_iterator_impl(Iterator x) -> decltype( make_skip_iterator<N>(x) ) {
  return make_skip_iterator<N>(x);
}

} // namespace impl

template<SkipIteratorStride N, typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN auto
skip(Range<Iterator, End, Count> const& x) -> decltype ( make_range(make_skip_iterator<N>(get_begin(x)), impl::make_skip_iterator_impl<N>(get_end(x)), get_count(x)/N) ) {

  static_assert(std::is_same<Count, Present>::value, "Count must be present to form a valid skip iterator");
  // Precondition get_count(x) % N == 0
  return make_range(make_skip_iterator<N>(get_begin(x)), impl::make_skip_iterator_impl<N>(get_end(x)), get_count(x)/N);
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN pair<Range<Iterator, Present, Present>, Range<Iterator, Present, Present>>
splitInTwo (Range<Iterator, End, Present> const& x) {
  return split_at(x, NotPresent{}, getCount(x)/2);
}

} // namespace range2

#endif
