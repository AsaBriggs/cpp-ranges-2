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

#ifndef INCLUDED_REVERSE
#include "reverse.h"
#endif

namespace range2 {

template<typename T0, typename T1>
struct pair
{
    typedef pair type;
    T0 m0;
    T1 m1;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type const& x, type const& y) { return (x.m0 == y.m0) && (x.m1 = y.m1); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!= (type const& x, type const& y) { return !(x == y); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator< (type const& x, type const& y) { return (x.m0 < y.m0) || ((!y.m0 < x.m0) && (x.m1 < y.m1)); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator<= (type const& x, type const& y) { return !(y < x); }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator> (type const& x, type const& y) { return y < x; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator>= (type const& x, type const& y) { return !(x < y); }
};

template<typename T0, typename T1>
ALWAYS_INLINE_HIDDEN pair<T0, T1> make_pair(T0 m0, T1 m1) {
  return {cmove(m0), cmove(m1)};
}



// cmove = constexpr move ... in C++11 std::move is not a constexpr!
template<typename T>
constexpr ALWAYS_INLINE_HIDDEN
typename std::remove_reference<T>::type&& cmove( T&& t ) {
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}

struct TYPE_DEFAULT_VISIBILITY NotPresent {
    typedef NotPresent type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!= (type, type) { return false; }
};

struct TYPE_DEFAULT_VISIBILITY Present {
    typedef Present type;

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator== (type, type) { return true; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    bool operator!= (type, type) { return false; }
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

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, NotPresent) {
        return {cmove(begin)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return false; }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return x.begin == y.begin; }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, NotPresent) {
        return {cmove(begin), cmove(end)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return x.begin == x.end; }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, RangeCountType<type> count) {
        return {cmove(begin), cmove(end), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return RangeCountType<type>(0) == x.count; }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, RangeCountType<type> count) {
        return {cmove(begin), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY GetPresence : Present {};

template<>
struct TYPE_HIDDEN_VISIBILITY GetPresence<NotPresent> : NotPresent {};

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type>
makeRange(Iterator begin, End end, Count count) {
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


template<typename T>
struct TYPE_HIDDEN_VISIBILITY HasEnd_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY HasEnd_Impl<Range<Iterator, End, Count>> : std::is_same<End, Present> {};

template<typename T>
using HasEnd = HasEnd_Impl<T>;

template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
getEnd(Range<Iterator, NotPresent, Count> const & x) {
  return {};
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasEnd<Range<Iterator, End, Count>>::value, Iterator&>::type
getEnd(Range<Iterator, End, Count>& x) {
    return x.end;
}

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasEnd<Range<Iterator, End, Count>>::value, Iterator const&>::type
getEnd(Range<Iterator, End, Count> const& x) {
    return x.end;
}


template<typename T>
struct TYPE_HIDDEN_VISIBILITY HasCount_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY HasCount_Impl<Range<Iterator, End, Count>> : std::is_same<Count, Present> {};

template<typename T>
using HasCount = HasCount_Impl<T>;


template<typename T>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!HasCount<T>::value, NotPresent>::type
getCount(T& x) {
  return {};
}

template<typename T>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasCount<T>::value, RangeCountType<T>&>::type
getCount(T& x) {
  return x.count;
}

template<typename T>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasCount<T>::value, RangeCountType<T> const&>::type
getCount(T const& x) {
  return x.count;
}

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


template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeAdvance : std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag> {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeDifference : std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag> {};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd : std::false_type {};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, Present, Count>> : std::true_type {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, NotPresent, Present>> : ConstantTimeAdvance<Iterator> {};

template<typename T>
struct CanAddEndInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and a count is present to terminate the loop.
template<typename Iterator, typename Count>
struct CanAddEndInLinearTime<Range<Iterator, NotPresent, Count>> :
  std::integral_constant<bool,
   !ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value &&
   std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
   !std::is_same<Count, NotPresent>::value>
{};


// Already present
template<typename Iterator, typename Count>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count>
addConstantTimeEnd(Range<Iterator, Present, Count> const& x) {
  return x;
}

// Can add in constant time
template<typename Iterator, typename Count>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count>>::value, Range<Iterator, Present, Count>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count> const& x) {
  auto tmp = x.begin;
  std::advance(tmp, x.count);
  return addEnd(x, cmove(tmp));
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
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount<Range<Iterator, Present, NotPresent>> : ConstantTimeDifference<Iterator> {};

template<typename T>
struct CanAddCountInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and a count is present to terminate the loop.
template<typename Iterator, typename End>
struct CanAddCountInLinearTime<Range<Iterator, End, NotPresent>> :
  std::integral_constant<bool,
  !ConstantTimeCount<Range<Iterator, End, NotPresent>>::value &&
  std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
  !std::is_same<End, NotPresent>::value>
{};


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
  return addCount(x, std::distance(x.begin, x.end));
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
  return addCount(addEnd(cmove(x), getBegin(tmp)), cmove(count));
}

template<typename Range, typename Middle, typename LHSCount, typename Enable=void>
struct TYPE_HIDDEN_VISIBILITY SplitImpl;

template<typename Iterator, typename End, typename Count>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, Count>, NotPresent, NotPresent>;

template<typename Iterator, typename End, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, NotPresent>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, NotPresent) -> decltype( make_pair(addEnd(x, middle), addBegin(x, middle)) ) {
    return make_pair(addEnd(x, middle), addBegin(x, middle));
  }
};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, NotPresent, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, NotPresent, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(addEnd(x, middle), addBegin(x, middle)) ) {
    CountType<Iterator> c = getCount(x);
    CountType<Iterator> diff = std::distance(getBegin(x), middle);
    c = c - std::distance(getBegin(x), middle);
    return make_pair(addEnd(addCount(x, diff), middle), addBegin(addCount(x, c), middle));
  }
};

template<typename Iterator, typename Middle>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, Present, Present>, Middle, NotPresent, typename std::enable_if<!std::is_same<NotPresent, Middle>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, Present, Present> const& x, Middle middle, NotPresent) -> decltype( make_pair(addEnd(removeCount(x), middle), addBegin(removeCount(x), middle)) ) {
     // No need to calculate the diff as the end iterator is present
     return make_pair(addEnd(removeCount(x), middle), addBegin(removeCount(x), middle));
  }
};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, Present>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, Present> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(addEnd(addCount(x, lhsCount), middle), addBegin(addCount(x, getCount(x) - lhsCount), middle)) ) {
    return make_pair(addEnd(addCount(x, lhsCount), middle), addBegin(addCount(x, getCount(x) - lhsCount), middle));
  }
};

template<typename Iterator, typename End, typename Middle, typename LHSCount>
struct TYPE_HIDDEN_VISIBILITY SplitImpl<Range<Iterator, End, NotPresent>, Middle, LHSCount, typename std::enable_if<!std::is_same<NotPresent, Middle>::value && !std::is_same<NotPresent, LHSCount>::value, void>::type> {
  static ALWAYS_INLINE_HIDDEN auto apply(Range<Iterator, End, NotPresent> const& x, Middle middle, LHSCount lhsCount) -> decltype( make_pair(addEnd(addCount(x, lhsCount), middle), addBegin(x, middle)) ) {
    return make_pair(addEnd(addCount(x, lhsCount), middle), addBegin(x, middle));
  }
};


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



ALWAYS_INLINE_HIDDEN NotPresent addCount(NotPresent, NotPresent) { return {}; }

template<typename Count0>
ALWAYS_INLINE_HIDDEN NotPresent addCount(Count0 const&, NotPresent) { return {}; }

template<typename Count1>
ALWAYS_INLINE_HIDDEN NotPresent addCount(NotPresent, Count1 const&) { return {}; }

template<typename Count0, typename Count1>
ALWAYS_INLINE_HIDDEN Count0 addCount(Count0 const& count0, Count1 const& count1) { return count0 + count1; }



// If the second range is bounded then the output range is bounded
template<typename Iterator0, typename End0, typename Count0, typename Iterator1, typename End1, typename Count1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, End1, Count1> const& y) -> decltype( makeRange(getBegin(x), getEnd(y), addCount(getCount(x), getCount(y))) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return makeRange(getBegin(x), getEnd(y), addCount(getCount(x), getCount(y)));
}

template<typename Iterator0, typename End0, typename Count0, typename Iterator1>
ALWAYS_INLINE_HIDDEN auto
join(Range<Iterator0, End0, Count0> const& x, Range<Iterator1, NotPresent, Present> const& y) -> decltype( join(x, addLinearTimeEnd(y)) ) {

  static_assert(std::is_convertible<Iterator1, Iterator0>::value, "Iterator types must be convertible");

  return join(x, addLinearTimeEnd(y));
}

// Reverse
// Skip ... toughie for non-divisible ranges

} // namespace range2

#endif
