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
using IteratorCategory = typename std::iterator_traits<Iterator>::iterator_category;

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
    bool operator!= (type const& x, type const& y) { return !(x == y); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return (x.m0 < y.m0) || (!(y.m0 < x.m0) && (x.m1 < y.m1)); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator<= (type const& x, type const& y) { return !(y < x); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator> (type const& x, type const& y) { return y < x; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator>= (type const& x, type const& y) { return !(x < y); }
};

template<typename T0, typename T1>
constexpr ALWAYS_INLINE_HIDDEN pair<T0, T1> make_pair(T0 m0, T1 m1) {
  return {cmove(m0), cmove(m1)};
}

struct TYPE_DEFAULT_VISIBILITY NotPresent {
    typedef NotPresent type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!= (type, type) { return false; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return false; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator<= (type const& x, type const& y) { return !(y < x); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator> (type const& x, type const& y) { return y < x; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator>= (type const& x, type const& y) { return !(x < y); }
};

struct TYPE_DEFAULT_VISIBILITY Present {
    typedef Present type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!= (type, type) { return false; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return false; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator<= (type const& x, type const& y) { return !(y < x); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator> (type const& x, type const& y) { return y < x; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator>= (type const& x, type const& y) { return !(x < y); }
};


template<typename Iterator, typename End=NotPresent, typename Count=NotPresent>
struct TYPE_DEFAULT_VISIBILITY Range;

template<typename T>
struct TYPE_HIDDEN_VISIBILITY IsABoundedRange_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY IsABoundedRange_Impl <Range<Iterator, End, Count>> : std::integral_constant<bool, !std::is_same<End, NotPresent>::value || ! std::is_same<Count, NotPresent>::value> {};

template<typename T>
using IsABoundedRange = IsABoundedRange_Impl<T>;


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
using RangeCountType = typename RangeIteratorTraits<T>::difference_type;

template<typename Iterator>
using CountType = typename std::iterator_traits<Iterator>::difference_type;

template<typename Iterator>
using IteratorCategory = typename std::iterator_traits<Iterator>::iterator_category;

template<typename T, typename Enable=void>
struct TYPE_HIDDEN_VISIBILITY RangeEffectiveIteratorCategory_Impl {};

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY RangeEffectiveIteratorCategory_Impl<Range<Iterator, End, Count>> {
  typedef IteratorCategory<Iterator> type;
};

template<typename T>
using RangeEffectiveIteratorCategory = typename RangeEffectiveIteratorCategory_Impl<T>::type;


template<typename Iterator>
constexpr ALWAYS_INLINE_HIDDEN Iterator advance(Iterator x) {
  return ++x;
}

constexpr ALWAYS_INLINE_HIDDEN NotPresent decrementCount(NotPresent) { return {}; }

template<typename Count>
constexpr ALWAYS_INLINE_HIDDEN Count decrementCount(Count x) { return x - 1; }

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

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!=(type const& x, type const& y) { return !(x==y); }
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

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, Iterator end, RangeCountType<type> count) {
        return {cmove(begin), cmove(end), cmove(count)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return RangeCountType<type>(0) == x.count; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN
    type make(Iterator begin, NotPresent, RangeCountType<type> count) {
        return {cmove(begin), cmove(count)};
    }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY GetPresence : Present {};

template<>
struct TYPE_HIDDEN_VISIBILITY GetPresence<NotPresent> : NotPresent {};

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type>
make_range(Iterator begin, End end, Count count) {
  static_assert(std::is_same<End, NotPresent>::value || std::is_convertible<End, Iterator>::value, "End Iterator must be NotPresent or of the same type as begin");
  static_assert(std::is_same<Count, NotPresent>::value || std::is_convertible<CountType<Iterator>, Count>::value, "Count must NotPresent or of type CountType<Iterator>");

  return Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type>::make(cmove(begin), cmove(end), cmove(count));
}


template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator&
getBegin(Range<Iterator, End, Count>& x) {
    return x.begin;
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator const&
getBegin(Range<Iterator, End, Count> const& x) {
    return x.begin;
}


template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
getEnd(Range<Iterator, NotPresent, Count> const& x) {
  return {};
}

template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator&
getEnd(Range<Iterator, Present, Count>& x) {
    return x.end;
}

template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Iterator const&
getEnd(Range<Iterator, Present, Count> const& x) {
    return x.end;
}


template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
getCount(Range<Iterator, End, NotPresent> const& x) {
  return {};
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN CountType<Iterator>&
getCount(Range<Iterator, End, Present>& x) {
  return x.count;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN CountType<Iterator> const&
getCount(Range<Iterator, End, Present> const& x) {
  return x.count;
}


template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN
bool operator< (Range<Iterator, End, Count> const& x, Range<Iterator, End, Count> const& y) {
  return (getBegin(x) < getBegin(y)) ||
         (!(getBegin(y) < getBegin(x)) && ((getEnd(x) < getEnd(y) ||
					    (!(getEnd(y) < getEnd(x)) && (getCount(x) < getCount(y))))));
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN
bool operator<= (Range<Iterator, End, Count> const& x, Range<Iterator, End, Count> const& y) { return !(y < x); }

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN
bool operator> (Range<Iterator, End, Count> const& x, Range<Iterator, End, Count> const& y) { return y < x; }

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN
bool operator>= (Range<Iterator, End, Count> const& x, Range<Iterator, End, Count> const& y) { return !(x < y); }



template<typename Iterator, typename End, typename Count, typename Iterator2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count>
addBegin(Range<Iterator, End, Count> const& x, Iterator2 begin) {
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "Begin iterator must be convertible to Range's Iterator");
  return Range<Iterator, End, Count>::make(begin, getEnd(x), getCount(x));
}

template<typename Iterator, typename End, typename Count, typename Iterator2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
addEnd(Range<Iterator, End, Count> const& x, Iterator2 end) {
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "End iterator must be convertible to Range's Iterator");
  return Range<Iterator, Present, Count>::make(getBegin(x), cmove(end), getCount(x));
}

template<typename Iterator, typename End, typename Count, typename Count2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present>
addCount(Range<Iterator, End, Count> const& x, Count2 count) {
  static_assert(std::is_convertible<Count2, CountType<Iterator>>::value, "Count must be convertible to Range's CountType");
  return Range<Iterator, End, Present>::make(getBegin(x), getEnd(x), count);
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, NotPresent, Count>
removeEnd(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, NotPresent, Count>::make(getBegin(x), {}, getCount(x));
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, NotPresent>
removeCount(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, End, NotPresent>::make(getBegin(x), getEnd(x), {});
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN bool
isEmpty(Range<Iterator, End, Count> const& x) {
  // ADL
  return empty_impl(x);
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count>
next(Range<Iterator, End, Count> const& x) {
  return Range<Iterator, End, Count>::make(advance(getBegin(x)), getEnd(x), decrementCount(getCount(x)));
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
addConstantTimeEnd(Range<Iterator, Present, Count> const& x) {
  return x;
}

// Can add in constant time
template<typename Iterator, typename Count>
constexpr typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Count>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count> const& x) {
  // Advance for randon access iterators is equivalent to pointer addition.
  return make_range(getBegin(x), advance(getBegin(x), getCount(x)), getCount(x));
}

// Can't add in constant time
template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, NotPresent, Count>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count> const& x) {
  return x;
}


template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
addLinearTimeEnd(Range<Iterator, Present, Count> const& x) {
  return x;
}

template<typename Iterator, typename Count>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Count>>::type
addLinearTimeEnd(Range<Iterator, NotPresent, Count> const& x) {
  return addConstantTimeEnd(x);
}

template<typename Iterator, typename Count>
typename std::enable_if<CanAddEndInLinearTime<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Present>>::type
addLinearTimeEnd(Range<Iterator, NotPresent, Count> const& x) {
  auto tmp = x;
  CountType<Iterator> count = 0;
  while(!isEmpty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return addCount(addEnd(cmove(x), getBegin(tmp)), cmove(count));
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
addConstantTimeCount(Range<Iterator, End, Present> const& x) {
  return x;
}

// Can add in constant time
template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, Present>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent> const& x) {
  return make_range(getBegin(x), getEnd(x), std::distance(getBegin(x), getEnd(x)));
}

// Can't add in constant time
template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, NotPresent>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent> const& x) {
  return x;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present>
addLinearTimeCount(Range<Iterator, End, Present> const& x) {
  return x;
}

template<typename Iterator, typename End>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent>>::value, Range<Iterator, End, Present>>::type
addLinearTimeCount(Range<Iterator, End, NotPresent> const& x) {
  return addConstantTimeCount(x);
}

template<typename Iterator, typename End>
ALWAYS_INLINE_HIDDEN typename std::enable_if<CanAddCountInLinearTime<Range<Iterator, End, NotPresent>>::value, Range<Iterator, Present, Present>>::type
addLinearTimeCount(Range<Iterator, End, NotPresent> const& x) {
  auto tmp = x;
  CountType<Iterator> count = 0;
  while(!isEmpty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return make_range(getBegin(x), getEnd(x), count);
}

struct TYPE_HIDDEN_VISIBILITY SplitAtImpl { typedef SplitAtImpl type; };


template<typename Range, typename Middle, typename LHSCount, typename Enable=void>
struct TYPE_HIDDEN_VISIBILITY SplitImpl;

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, Count>, NotPresent, NotPresent>;

template<typename Iterator, typename End, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, NotPresent>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(getBegin(x), middle, NotPresent{}), make_range(middle, getEnd(x), NotPresent{})) ) {
    return make_pair(make_range(getBegin(x), middle, NotPresent{}), make_range(middle, getEnd(x), NotPresent{}));
  }
};

template<typename Iterator, typename End, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, End, NotPresent>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : ConstantComplexity {};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, NotPresent, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, NotPresent, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(getBegin(x), middle, CountType<Iterator>()), make_range(middle, NotPresent{}, CountType<Iterator>()))) {
    CountType<Iterator> diff = std::distance(getBegin(x), middle);
    CountType<Iterator> c = getCount(x) - diff;
    return make_pair(make_range(getBegin(x), middle, diff), make_range(middle, NotPresent{}, c));
  }
};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, NotPresent, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : Complexity<Distance, Iterator> {};


template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, Present, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, Present, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(make_range(getBegin(x), middle, NotPresent{}), make_range(middle, getEnd(x), NotPresent{})) ) {
     // No need to calculate the diff as the end iterator is present
    return make_pair(make_range(getBegin(x), middle, NotPresent{}), make_range(middle, getEnd(x), NotPresent{}));
  }
};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, Present, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> : ConstantComplexity {};


template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, Present>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, Present> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(make_range(getBegin(x), middle, lhsCount), make_range(middle, getEnd(x), getCount(x) - lhsCount)) ) {
    return make_pair(make_range(getBegin(x), middle, lhsCount), make_range(middle, getEnd(x), getCount(x) - lhsCount));
  }
};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, End, Present>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> : ConstantComplexity {};


template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, NotPresent>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(make_range(getBegin(x), middle, lhsCount), make_range(middle, getEnd(x), NotPresent{})) ) {
    return make_pair(make_range(getBegin(x), middle, lhsCount), make_range(middle, getEnd(x), NotPresent{}));
  }
};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, End, NotPresent>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> : ConstantComplexity {};


template<typename Iterator, typename End, typename Count, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, Count>, NotPresent, LHSCount, typename std::enable_if<!std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, Count> const& x, NotPresent, LHSCount lhsCount) -> decltype(SplitImpl<Range<Iterator, End, Count>, Iterator, LHSCount>::apply(x, getBegin(x), cmove(lhsCount)) ) {
    // We must calculate the middle in order for the second range to be able to start somewhere.
    Iterator middle = getBegin(x) ;
    std::advance(middle, lhsCount);
    // Not really recursion as the SplitImpl type is different (Middle is now present).
    return SplitImpl<Range<Iterator, End, Count>, Iterator, LHSCount>::apply(x, cmove(middle), cmove(lhsCount));
  } 
};

template<typename Iterator, typename End, typename Count, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAtImpl, Range<Iterator, End, Count>, NotPresent, LHSCount, typename std::enable_if<!std::is_same<NotPresent, LHSCount>::value, void>::type> : Complexity<Advance, Iterator> {};


// If input range is bounded then both returned ranges are bounded.
// Otherwise if input range in unbounded then first returned range is bounded and second returned range is unbounded.
template<typename Iterator, typename End, typename Count, typename Middle, typename LHSCount>
ALWAYS_INLINE_HIDDEN auto
splitAt(Range<Iterator, End, Count> const& x, Middle middle, LHSCount lhsCount)
    -> decltype(SplitImpl<Range<Iterator, End, Count>, Middle, LHSCount>::apply(x, cmove(middle), cmove(lhsCount))) {

  static_assert(std::is_same<Middle, NotPresent>::value || std::is_convertible<Middle, Iterator>::value, "Middle Iterator must be NotPresent or convertible to the same type as begin");
  static_assert(std::is_same<LHSCount, NotPresent>::value || std::is_convertible<CountType<Iterator>, LHSCount>::value, "LHSCount must NotPresent or convertible to type CountType<Iterator>");
  static_assert(!std::is_same<Middle, NotPresent>::value || !std::is_same<LHSCount, NotPresent>::value, "At least a middle or an lhs count must be supplied");

  return SplitImpl<Range<Iterator, End, Count>, Middle, LHSCount>::apply(x, cmove(middle), cmove(lhsCount));
}

struct TYPE_HIDDEN_VISIBILITY SplitAt { typedef SplitAt type; };
template<typename Iterator, typename End, typename Count, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY Complexity<SplitAt, Range<Iterator, End, Count>, Middle, LHSCount> : Complexity<SplitAtImpl, Range<Iterator, End, Count>, Middle, LHSCount> {};



ALWAYS_INLINE_HIDDEN NotPresent sumCount(NotPresent, NotPresent) { return {}; }

template<typename Count0>
ALWAYS_INLINE_HIDDEN NotPresent sumCount(Count0 const&, NotPresent) { return {}; }

template<typename Count1>
ALWAYS_INLINE_HIDDEN NotPresent sumCount(NotPresent, Count1 const&) { return {}; }

template<typename Count0, typename Count1>
ALWAYS_INLINE_HIDDEN Count0 sumCount(Count0 const& count0, Count1 const& count1) { return count0 + count1; }



// If the second range is bounded then the output range is bounded
template<typename Iterator0, typename End0, typename Count0, typename Iterator1, typename End1, typename Count1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, End1, Count1> const& y) -> decltype( make_range(getBegin(x), getEnd(y), sumCount(getCount(x), getCount(y))) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return make_range(getBegin(x), getEnd(y), sumCount(getCount(x), getCount(y)));
}

struct TYPE_HIDDEN_VISIBILITY Join { typedef Join type; };

template<typename Iterator0, typename End0, typename Count0, typename Iterator1, typename End1, typename Count1>
struct TYPE_HIDDEN_VISIBILITY Complexity<Join, Range<Iterator0, End0, Count0>, Range<Iterator1, End1, Count1>> : ConstantComplexity {};


template<typename Iterator0, typename End0, typename Count0, typename Iterator1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, NotPresent, Present> const& y) -> decltype( join(x, addLinearTimeEnd(y)) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return join(x, addLinearTimeEnd(y));
}

template<typename Iterator0, typename End0, typename Count0, typename Iterator1>
struct TYPE_HIDDEN_VISIBILITY Complexity<Join, Range<Iterator0, End0, Count0>, Range<Iterator1, NotPresent, Present>> : Complexity<AddEnd, Range<Iterator1, NotPresent, Present>> {};


struct TYPE_HIDDEN_VISIBILITY Reverse_Impl { typedef Reverse_Impl type; };

template<typename T>
struct TYPE_HIDDEN_VISIBILITY reverse_impl;

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY reverse_impl<Range<Iterator, Present, Count>> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, Present, Count> const& x) -> decltype( make_range(make_reverse_iterator<Iterator>(getEnd(x)), make_reverse_iterator<Iterator>(getBegin(x)), getCount(x)) ) {
    return make_range(make_reverse_iterator<Iterator>(getEnd(x)), make_reverse_iterator<Iterator>(getBegin(x)), getCount(x));
  }
};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse_Impl, Range<Iterator, Present, Count>> : ConstantComplexity {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY reverse_impl<Range<Iterator, NotPresent, Present>> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, NotPresent, Present> const& x) -> decltype( reverse_impl<Range<Iterator, Present, Present>>::apply(addLinearTimeEnd(x)) ) {
    return reverse_impl<Range<Iterator, Present, Present>>::apply(addLinearTimeEnd(x));
  }
};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse_Impl, Range<Iterator, NotPresent, Present>> : Complexity<AddEnd, Range<Iterator, NotPresent, Present>> {};


template<typename Iterator, typename End, typename Count>
ALWAYS_INLINE_HIDDEN auto
reverse(Range<Iterator, End, Count> const& x) -> decltype ( reverse_impl<Range<Iterator, End, Count>>::apply(x) ) {

  static_assert(IsABoundedRange<Range<Iterator, End, Count>>::value, "Must be a bounded range");

  return reverse_impl<Range<Iterator, End, Count>>::apply(x);
}

struct TYPE_HIDDEN_VISIBILITY Reverse { typedef Reverse type; };

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY Complexity<Reverse, Range<Iterator, End, Count>> : Complexity<Reverse_Impl, Range<Iterator, End, Count>> {};


template<long long N>
ALWAYS_INLINE_HIDDEN NotPresent make_skip_iterator_impl(NotPresent x) {
  return x;
}

template<long long N, typename Iterator>
ALWAYS_INLINE_HIDDEN auto make_skip_iterator_impl(Iterator x) -> decltype( make_skip_iterator<N>(x) ) {
  return make_skip_iterator<N>(x);
}

template<long long N, typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN auto
skip(Range<Iterator, End, Count> const& x) -> decltype ( make_range(make_skip_iterator<N>(getBegin(x)), make_skip_iterator_impl<N>(getEnd(x)), getCount(x)/N) ) {

  static_assert(std::is_same<Count, Present>::value, "Count must be present to form a valid skip iterator");
  // Precondition getCount(x) % N == 0
  return make_range(make_skip_iterator<N>(getBegin(x)), make_skip_iterator_impl<N>(getEnd(x)), getCount(x)/N);
}

} // namespace range2

#endif
