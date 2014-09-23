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

// cmove = constexpr move ... in C++11 std::move is not a constexpr!
template<typename T>
constexpr inline __attribute__ ((visibility("hidden")))
typename std::remove_reference<T>::type&& cmove( T&& t ) {
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}

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

    static constexpr type
    make(Iterator begin, NotPresent, NotPresent, NotPresent)
    {
        return {cmove(begin)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, NotPresent, NotPresent)
    {
        return {cmove(begin), cmove(end)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, RangeCountType<type> count, NotPresent)
    {
        return {cmove(begin), cmove(end), cmove(count)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, NotPresent, Predicate p)
    {
        return {cmove(begin), cmove(end), cmove(p)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, NotPresent, Predicate p)
    {
      return {cmove(begin), cmove(end)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, RangeCountType<type> count, Predicate p)
    {
        return {cmove(begin), cmove(end), cmove(count), cmove(p)};
    }

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

    static constexpr type
    make(Iterator begin, Iterator end, RangeCountType<type> count, Predicate p)
    {
      return {cmove(begin), cmove(end), cmove(count)};
    }

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

    static constexpr type
      make(Iterator begin, NotPresent, RangeCountType<type> count, NotPresent)
    {
        return {cmove(begin), cmove(count)};
    }

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

    static constexpr type
    make(Iterator begin, NotPresent, RangeCountType<type> count, Predicate p)
    {
      return {cmove(begin), cmove(count), cmove(p)};
    }

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

    static constexpr type
    make(Iterator begin, NotPresent, RangeCountType<type> count, Predicate p)
    {
        return {cmove(begin), cmove(count)};
    }

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

    static constexpr type
    make(Iterator begin, NotPresent, NotPresent, Predicate p)
    {
        return {cmove(begin), cmove(p)};
    }

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.p == y.p); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct Range<Iterator, NotPresent, NotPresent, Predicate, typename std::enable_if<!StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;

    static constexpr type
    make(Iterator begin, NotPresent, NotPresent, Predicate p)
    {
      return {cmove(begin)};
    }

    friend
    constexpr bool operator==(type const& x, type const& y) { return (x.begin == y.begin); }

    friend
    constexpr bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Range<Iterator, End, Count, Predicate>
makeRange(Iterator begin, End end, Count count, Predicate predicate) {
  static_assert(std::is_same<End, NotPresent>::value || std::is_same<End, Iterator>::value, "End Iterator must be NotPresent or of the same type as begin");
  static_assert(std::is_same<Count, NotPresent>::value || std::is_same<Count, CountType<Iterator>>::value, "Count must NotPresent or of type CountType<Iterator>");

  return Range<Iterator, End, Count, Predicate>::make(begin, end, count, predicate);
}



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

template<typename Iterator, typename Count, typename Predicate>
constexpr NotPresent
getEnd(Range<Iterator, NotPresent, Count, Predicate> const & x) {
  return {};
}

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
constexpr typename std::enable_if<!HasCount<T>::value, NotPresent>::type
getCount(T& x) {
  return {};
}

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

template<typename Iterator, typename End, typename Count>
constexpr NotPresent
getPredicate(Range<Iterator, End, Count, NotPresent> const&) {
    return {};
}

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



template<typename Iterator, typename End, typename Count, typename Predicate, typename Iterator2>
constexpr Range<Iterator, Present, Count, Predicate>
addEnd(Range<Iterator, End, Count, Predicate> const& x, Iterator2 end)
{
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "End iterator must be convertible to Range's Iterator");
  return Range<Iterator, Present, Count, Predicate>::make(getBegin(x), cmove(end), getCount(x), getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate, typename Count2>
constexpr Range<Iterator, End, Present, Predicate>
addCount(Range<Iterator, End, Count, Predicate> const& x, Count2 count)
{
  static_assert(std::is_convertible<Count2, CountType<Iterator>>::value, "Count must be convertible to Range's CountType");
  return Range<Iterator, End, Present, Predicate>::make(getBegin(x), getEnd(x), count, getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate, typename Predicate2>
constexpr Range<Iterator, End, Count, Predicate2>
addPredicate(Range<Iterator, End, Count, Predicate> const& x, Predicate2 p)
{
  return Range<Iterator, End, Count, Predicate2>::make(getBegin(x), getEnd(x), getCount(x), p);
}


template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Range<Iterator, NotPresent, Count, Predicate>
removeEnd(Range<Iterator, End, Count, Predicate> const& x)
{
  return Range<Iterator, NotPresent, Count, Predicate>::make(getBegin(x), {}, getCount(x), getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Range<Iterator, End, NotPresent, Predicate>
removeCount(Range<Iterator, End, Count, Predicate> const& x)
{
  return Range<Iterator, End, NotPresent, Predicate>::make(getBegin(x), getEnd(x), {}, getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr Range<Iterator, End, Count, NotPresent>
removePredicate(Range<Iterator, End, Count, Predicate> const& x)
{
  return Range<Iterator, End, Count, NotPresent>::make(getBegin(x), getEnd(x), getCount(x), {});
}





template<typename Iterator>
struct ConstantTimeAdvance : std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag> {};

template<typename Iterator>
struct ConstantTimeDifference : std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag> {};


template<typename T>
struct ConstantTimeEnd : std::false_type {};

template<typename Iterator, typename Count>
struct ConstantTimeEnd<Range<Iterator, Present, Count, NotPresent>> : std::true_type {};

template<typename Iterator>
struct ConstantTimeEnd<Range<Iterator, NotPresent, Present, NotPresent>> : ConstantTimeAdvance<Iterator> {};

// Already present
template<typename Iterator, typename Count, typename Predicate>
constexpr Range<Iterator, Present, Count, Predicate>
addConstantTimeEnd(Range<Iterator, Present, Count, Predicate> const& x)
{
  return x;
}

// Can add in constant time (ends up that Predicate = NotPresent)
template<typename Iterator, typename Count, typename Predicate>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, Present, Count, Predicate>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x)
{
  auto tmp = x.begin;
  std::advance(tmp, x.count);
  return addEnd(x, tmp);
}

// Can't add in constant time
template<typename Iterator, typename Count, typename Predicate>
constexpr typename std::enable_if<!ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, NotPresent, Count, Predicate>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x)
{
  return x;
}


template<typename T>
struct ConstantTimeCount : std::false_type {};

template<typename Iterator, typename End>
struct ConstantTimeCount<Range<Iterator, End, Present, NotPresent>> : std::true_type {};

template<typename Iterator>
struct ConstantTimeCount<Range<Iterator, Present, NotPresent, NotPresent>> : ConstantTimeDifference<Iterator> {};

// Already present
template<typename Iterator, typename End, typename Predicate>
constexpr Range<Iterator, End, Present, Predicate>
addConstantTimeCount(Range<Iterator, End, Present, Predicate> const& x)
{
  return x;
}

// Can add in constant time (ends up that Predicate = NotPresent)
template<typename Iterator, typename End, typename Predicate>
constexpr typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, End, Present, Predicate>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x)
{
  return addCount(x, std::distance(x.begin, x.end));
}

// Can't add in constant time
template<typename Iterator, typename End, typename Predicate>
constexpr typename std::enable_if<!ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, End, NotPresent, Predicate>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x)
{
  return x;
}

} // namespace range2

#endif
