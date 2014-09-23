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

namespace range2 {

// To use std::true_type/std::false_type or these custom types? Custom = more searchable.
struct NotPresent { typedef NotPresent type; };
struct Present { typedef Present type; };


// Optimise .. don't store the predicate unless it is stateful.
// Do this by only specialising storage for when Predicate is Present and stateful, wrapping the getting of the predicate
// with a layer of enable_if.
template<typename Predicate>
struct StorePredicate : std::integral_constant<bool, !std::is_empty<Predicate>::value> {};


template<typename Iterator, typename End=NotPresent, typename Count=NotPresent, typename Predicate=NotPresent, typename Enable=void>
struct Range;

template<typename T>
struct RangeIterator_Impl;

template<typename Iterator, typename End, typename Count, typename Predicate>
struct RangeIterator_Impl<Range<Iterator, End, Count, Predicate>>
{
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


template<typename Iterator, typename End, typename Count, typename Predicate, typename Enable>
struct Range
{
    typedef Range type;
    Iterator begin;

    friend
    constexpr bool operator==(type const& x, type const& y) { return x.begin == y.begin; }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator>
struct Range<Iterator, Present, NotPresent, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator>
struct Range<Iterator, Present, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, Present, NotPresent, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    Predicate p;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.p == y.p); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, Present, NotPresent, Predicate, typename std::enable_if<!StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    Predicate p;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator, typename Predicate>
struct Range<Iterator, Present, Present, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;
    Predicate p;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count) && (x.p == y.p); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, Present, Present, Predicate, typename std::enable_if<!StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator>
struct Range<Iterator, NotPresent, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator, typename Predicate>
struct Range<Iterator, NotPresent, Present, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;
    Predicate p;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count) && (x.p == y.p); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, NotPresent, Present, Predicate, typename std::enable_if<!StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, NotPresent, NotPresent, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Predicate p;

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.p == y.p); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Iterator&
getBegin(Range<Iterator, End, Count, Predicate>& x) {
    return x.begin;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Iterator const&
getBegin(Range<Iterator, End, Count, Predicate> const& x) {
    return x.begin;
}


template<typename T>
struct HasEnd_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct HasEnd_Impl<Range<Iterator, End, Count, Predicate>> : std::is_same<End, Present> {};

template<typename T>
using HasEnd = HasEnd_Impl<T>;

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr typename std::enable_if<HasEnd<Range<Iterator, End, Count, Predicate>>::value, Iterator&>::type
getEnd(Range<Iterator, End, Count, Predicate>& x) {
    return x.end;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr typename std::enable_if<HasEnd<Range<Iterator, End, Count, Predicate>>::value, Iterator const&>::type
getEnd(Range<Iterator, End, Count, Predicate> const& x) {
    return x.end;
}


template<typename T>
struct HasCount_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct HasCount_Impl<Range<Iterator, End, Count, Predicate>> : std::is_same<Count, Present> {};

template<typename T>
using HasCount = HasCount_Impl<T>;

template<typename T>
constexpr typename std::enable_if<HasCount<T>::value, RangeCountType<T>&>::type
getCount(T& x) {
    return x.count;
}

template<typename T>
constexpr typename std::enable_if<HasCount<T>::value, RangeCountType<T> const&>::type
getCount(T const& x) {
    return x.count;
}


template<typename T>
struct HasPredicate_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct HasPredicate_Impl<Range<Iterator, End, Count, Predicate>> : std::integral_constant<bool, !std::is_same<Predicate, NotPresent>::value> {};

template<typename T>
using HasPredicate = HasPredicate_Impl<T>;

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && !StorePredicate<Predicate>::value, Predicate>::type
getPredicate(Range<Iterator, End, Count, Predicate> const&) {
    return {};
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && StorePredicate<Predicate>::value, Predicate const&>::type
getPredicate(Range<Iterator, End, Count, Predicate> const& x) {
    return x.p;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && StorePredicate<Predicate>::value, Predicate&>::type
getPredicate(Range<Iterator, End, Count, Predicate>& x) {
    return x.p;
}


template<typename Iterator, typename Iterator2, typename Predicate>
constexpr typename std::enable_if<std::is_same<Predicate, NotPresent>::value || !StorePredicate<Predicate>::value, Range<Iterator, Present, NotPresent, Predicate>>::type
addEnd(Range<Iterator, NotPresent, NotPresent, Predicate> x, Iterator2 end)
{
  static_assert(std::is_convertible<Iterator2, Iterator>(), "End must be convertible to Range's Iterator");
  return {x.begin, end};
}

template<typename Iterator, typename Iterator2, typename Predicate>
constexpr typename std::enable_if<!std::is_same<Predicate, NotPresent>::value && StorePredicate<Predicate>::value, Range<Iterator, Present, NotPresent, Predicate>>::type
addEnd(Range<Iterator, NotPresent, NotPresent, Predicate> x, Iterator2 end)
{
  static_assert(std::is_convertible<Iterator2, Iterator>(), "End must be convertible to Range's Iterator");
  return {x.begin, end, x.p};
}

template<typename Iterator, typename Iterator2, typename Predicate>
constexpr typename std::enable_if<std::is_same<Predicate, NotPresent>::value || !StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addEnd(Range<Iterator, NotPresent, Present, Predicate> x, Iterator2 end)
{
  static_assert(std::is_convertible<Iterator2, Iterator>(), "End must be convertible to Range's Iterator");
  return {x.begin, end, x.count};
}

template<typename Iterator, typename Iterator2, typename Predicate>
constexpr typename std::enable_if<!std::is_same<Predicate, NotPresent>::value && StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addEnd(Range<Iterator, NotPresent, Present, Predicate> x, Iterator2 end)
{
  static_assert(std::is_convertible<Iterator2, Iterator>(), "End must be convertible to Range's Iterator");
  return {x.begin, end, x.count, x.p};
}


template<typename Iterator, typename Count, typename Predicate>
constexpr typename std::enable_if<std::is_same<Predicate, NotPresent>::value || !StorePredicate<Predicate>::value, Range<Iterator, NotPresent, Present, Predicate>>::type
addCount(Range<Iterator, NotPresent, NotPresent, Predicate> x, Count count)
{
  static_assert(std::is_convertible<Count, CountType<Iterator>>(), "Count must be convertible to Range's CountType");
  return {x.begin, count};
}

template<typename Iterator, typename Count, typename Predicate>
constexpr typename std::enable_if<!std::is_same<Predicate, NotPresent>::value && StorePredicate<Predicate>::value, Range<Iterator, NotPresent, Present, Predicate>>::type
addCount(Range<Iterator, NotPresent, NotPresent, Predicate> x, Count count)
{
  static_assert(std::is_convertible<Count, CountType<Iterator>>(), "Count must be convertible to Range's CountType");
  return {x.begin, count, x.p};
}

template<typename Iterator, typename Count, typename Predicate>
constexpr typename std::enable_if<std::is_same<Predicate, NotPresent>::value || !StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addCount(Range<Iterator, Present, NotPresent, Predicate> x, Count count)
{
  static_assert(std::is_convertible<Count, CountType<Iterator>>(), "Count must be convertible to Range's CountType");
  return {x.begin, x.end, count};
}

template<typename Iterator, typename Count, typename Predicate>
constexpr typename std::enable_if<!std::is_same<Predicate, NotPresent>::value && StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addCount(Range<Iterator, Present, NotPresent, Predicate> x, Count count)
{
  static_assert(std::is_convertible<Count, CountType<Iterator>>(), "Count must be convertible to Range's CountType");
  return {x.begin, x.end, count, x.p};
}


template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<StorePredicate<Predicate>::value, Range<Iterator, NotPresent, NotPresent, Predicate>>::type
addPredicate(Range<Iterator, NotPresent, NotPresent, NotPresent> x, Predicate p)
{
  return {x.begin, p};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<!StorePredicate<Predicate>::value, Range<Iterator, NotPresent, NotPresent, Predicate>>::type
addPredicate(Range<Iterator, NotPresent, NotPresent, NotPresent> x, Predicate p)
{
  return {x.begin};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<StorePredicate<Predicate>::value, Range<Iterator, NotPresent, Present, Predicate>>::type
addPredicate(Range<Iterator, NotPresent, Present, NotPresent> x, Predicate p)
{
  return {x.begin, x.count, p};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<!StorePredicate<Predicate>::value, Range<Iterator, NotPresent, Present, Predicate>>::type
addPredicate(Range<Iterator, NotPresent, Present, NotPresent> x, Predicate p)
{
  return {x.begin, x.count};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<StorePredicate<Predicate>::value, Range<Iterator, Present, NotPresent, Predicate>>::type
addPredicate(Range<Iterator, Present, NotPresent, NotPresent> x, Predicate p)
{
  return {x.begin, x.end, p};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<!StorePredicate<Predicate>::value, Range<Iterator, Present, NotPresent, Predicate>>::type
addPredicate(Range<Iterator, Present, NotPresent, NotPresent> x, Predicate p)
{
  return {x.begin, x.end};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addPredicate(Range<Iterator, Present, Present, NotPresent> x, Predicate p)
{
  return {x.begin, x.end, x.count, p};
}

template<typename Iterator, typename Predicate>
constexpr typename std::enable_if<!StorePredicate<Predicate>::value, Range<Iterator, Present, Present, Predicate>>::type
addPredicate(Range<Iterator, Present, Present, NotPresent> x, Predicate p)
{
  return {x.begin, x.end, x.count};
}

} // namespace range2

#endif
