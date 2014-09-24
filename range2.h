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

namespace range2 {

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


// Optimise .. don't store the predicate unless it is stateful.
// Do this by only specialising storage for when Predicate is Present and stateful, wrapping the getting of the predicate
// with a layer of enable_if.
template<typename Predicate>
struct TYPE_HIDDEN_VISIBILITY StorePredicate : std::integral_constant<bool, !std::is_empty<Predicate>::value> {};


template<typename Iterator, typename End=NotPresent, typename Count=NotPresent, typename Predicate=NotPresent, typename Enable=void>
struct TYPE_DEFAULT_VISIBILITY Range;

template<typename T>
struct TYPE_HIDDEN_VISIBILITY RangeIterator_Impl;

template<typename Iterator, typename End, typename Count, typename Predicate>
struct TYPE_HIDDEN_VISIBILITY RangeIterator_Impl<Range<Iterator, End, Count, Predicate>> {
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
struct TYPE_HIDDEN_VISIBILITY RangeEffectiveIteratorCategory_Impl<Range<Iterator, End, Count, NotPresent>> {
  typedef IteratorCategory<Iterator> type;
};

// Having a predicate present means that iterator_category > forward are reduced to being forward.
template<typename Iterator, typename End, typename Count, typename Predicate>
  struct TYPE_HIDDEN_VISIBILITY RangeEffectiveIteratorCategory_Impl<Range<Iterator, End, Count, Predicate>, typename std::enable_if<std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value && !std::is_same<Predicate, NotPresent>::value, void>::type> {
  typedef std::forward_iterator_tag type;
};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct TYPE_HIDDEN_VISIBILITY RangeEffectiveIteratorCategory_Impl<Range<Iterator, End, Count, Predicate>, typename std::enable_if<!std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value && !std::is_same<Predicate, NotPresent>::value, void>::type> {
  typedef std::input_iterator_tag type;
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

template<typename Iterator, typename End, typename Count, typename Predicate, typename Enable>
struct TYPE_DEFAULT_VISIBILITY Range
{
    typedef Range type;
    Iterator begin;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, NotPresent, NotPresent) {
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
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, NotPresent, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, NotPresent, NotPresent) {
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
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, RangeCountType<type> count, NotPresent) {
        return {cmove(begin), cmove(end), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return RangeCountType<type>(0) == x.count; }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, NotPresent, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    Predicate p;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, NotPresent, Predicate p) {
        return {cmove(begin), cmove(end), cmove(p)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (x.begin == x.end) || x.p(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.p == y.p); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, NotPresent, Predicate, typename std::enable_if<!std::is_same<NotPresent, Predicate>::value && !StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, NotPresent, Predicate p) {
      return {cmove(begin), cmove(end)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (x.begin == x.end) || Predicate{}(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;
    Predicate p;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, RangeCountType<type> count, Predicate p) {
        return {cmove(begin), cmove(end), cmove(count), cmove(p)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count) || x.p(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count) && (x.p == y.p); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, Present, Present, Predicate, typename std::enable_if<!std::is_same<NotPresent, Predicate>::value && !StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Iterator end;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, Iterator end, RangeCountType<type> count, Predicate p) {
      return {cmove(begin), cmove(end), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count) || Predicate{}(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.end == y.end) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present, NotPresent>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, RangeCountType<type> count, NotPresent) {
        return {cmove(begin), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};


template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;
    Predicate p;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, RangeCountType<type> count, Predicate p) {
      return {cmove(begin), cmove(count), cmove(p)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count) || x.p(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count) && (x.p == y.p); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, Present, Predicate, typename std::enable_if<!std::is_same<NotPresent, Predicate>::value && !StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    RangeCountType<type> count;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, RangeCountType<type> count, Predicate p) {
        return {cmove(begin), cmove(count)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return (RangeCountType<type>(0) == x.count) || Predicate{}(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.count == y.count); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, NotPresent, Predicate, typename std::enable_if<StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;
    Predicate p;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, NotPresent, Predicate p) {
        return {cmove(begin), cmove(p)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return x.p(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin) && (x.p == y.p); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename Iterator, typename Predicate>
struct TYPE_DEFAULT_VISIBILITY Range<Iterator, NotPresent, NotPresent, Predicate, typename std::enable_if<!std::is_same<NotPresent, Predicate>::value && !StorePredicate<Predicate>::value, void>::type>
{
    typedef Range type;
    Iterator begin;

    static constexpr ALWAYS_INLINE_HIDDEN type
    make(Iterator begin, NotPresent, NotPresent, Predicate p) {
      return {cmove(begin)};
    }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool empty_impl(type const& x) { return Predicate{}(x.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator==(type const& x, type const& y) { return (x.begin == y.begin); }

    friend
    constexpr ALWAYS_INLINE_HIDDEN bool operator!=(type const& x, type const& y) { return !(x==y); }
};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY GetPresence : Present {};

template<>
struct TYPE_HIDDEN_VISIBILITY GetPresence<NotPresent> : NotPresent {};

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type, Predicate>
makeRange(Iterator begin, End end, Count count, Predicate predicate) {
  static_assert(std::is_same<End, NotPresent>::value || std::is_convertible<End, Iterator>::value, "End Iterator must be NotPresent or of the same type as begin");
  static_assert(std::is_same<Count, NotPresent>::value || std::is_convertible<CountType<Iterator>, Count>::value, "Count must NotPresent or of type CountType<Iterator>");

  return Range<Iterator, typename GetPresence<End>::type, typename GetPresence<Count>::type, Predicate>::make(begin, end, count, predicate);
}



template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Iterator&
getBegin(Range<Iterator, End, Count, Predicate>& x) {
    return x.begin;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Iterator const&
getBegin(Range<Iterator, End, Count, Predicate> const& x) {
    return x.begin;
}


template<typename T>
struct TYPE_HIDDEN_VISIBILITY HasEnd_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct TYPE_HIDDEN_VISIBILITY HasEnd_Impl<Range<Iterator, End, Count, Predicate>> : std::is_same<End, Present> {};

template<typename T>
using HasEnd = HasEnd_Impl<T>;

template<typename Iterator, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
getEnd(Range<Iterator, NotPresent, Count, Predicate> const & x) {
  return {};
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasEnd<Range<Iterator, End, Count, Predicate>>::value, Iterator&>::type
getEnd(Range<Iterator, End, Count, Predicate>& x) {
    return x.end;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasEnd<Range<Iterator, End, Count, Predicate>>::value, Iterator const&>::type
getEnd(Range<Iterator, End, Count, Predicate> const& x) {
    return x.end;
}


template<typename T>
struct TYPE_HIDDEN_VISIBILITY HasCount_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct TYPE_HIDDEN_VISIBILITY HasCount_Impl<Range<Iterator, End, Count, Predicate>> : std::is_same<Count, Present> {};

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


template<typename T>
struct TYPE_HIDDEN_VISIBILITY HasPredicate_Impl : std::false_type {};

template<typename Iterator, typename End, typename Count, typename Predicate>
struct TYPE_HIDDEN_VISIBILITY HasPredicate_Impl<Range<Iterator, End, Count, Predicate>> : std::integral_constant<bool, !std::is_same<Predicate, NotPresent>::value> {};

template<typename T>
using HasPredicate = HasPredicate_Impl<T>;

template<typename Iterator, typename End, typename Count>
constexpr ALWAYS_INLINE_HIDDEN NotPresent
getPredicate(Range<Iterator, End, Count, NotPresent> const&) {
    return {};
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && !StorePredicate<Predicate>::value, Predicate>::type
getPredicate(Range<Iterator, End, Count, Predicate> const&) {
    return {};
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && StorePredicate<Predicate>::value, Predicate const&>::type
getPredicate(Range<Iterator, End, Count, Predicate> const& x) {
    return x.p;
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<HasPredicate<Range<Iterator, End, Count, Predicate>>::value && StorePredicate<Predicate>::value, Predicate&>::type
getPredicate(Range<Iterator, End, Count, Predicate>& x) {
    return x.p;
}



template<typename Iterator, typename End, typename Count, typename Predicate, typename Iterator2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count, Predicate>
addEnd(Range<Iterator, End, Count, Predicate> const& x, Iterator2 end) {
  static_assert(std::is_convertible<Iterator2, Iterator>::value, "End iterator must be convertible to Range's Iterator");
  return Range<Iterator, Present, Count, Predicate>::make(getBegin(x), cmove(end), getCount(x), getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate, typename Count2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present, Predicate>
addCount(Range<Iterator, End, Count, Predicate> const& x, Count2 count) {
  static_assert(std::is_convertible<Count2, CountType<Iterator>>::value, "Count must be convertible to Range's CountType");
  return Range<Iterator, End, Present, Predicate>::make(getBegin(x), getEnd(x), count, getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate, typename Predicate2>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count, Predicate2>
addPredicate(Range<Iterator, End, Count, Predicate> const& x, Predicate2 p) {
  return Range<Iterator, End, Count, Predicate2>::make(getBegin(x), getEnd(x), getCount(x), p);
}


template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, NotPresent, Count, Predicate>
removeEnd(Range<Iterator, End, Count, Predicate> const& x) {
  return Range<Iterator, NotPresent, Count, Predicate>::make(getBegin(x), {}, getCount(x), getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, NotPresent, Predicate>
removeCount(Range<Iterator, End, Count, Predicate> const& x) {
  return Range<Iterator, End, NotPresent, Predicate>::make(getBegin(x), getEnd(x), {}, getPredicate(x));
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count, NotPresent>
removePredicate(Range<Iterator, End, Count, Predicate> const& x) {
  return Range<Iterator, End, Count, NotPresent>::make(getBegin(x), getEnd(x), getCount(x), {});
}


template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN bool
isEmpty(Range<Iterator, End, Count, Predicate> const& x) {
  // ADL
  return empty_impl(x);
}

template<typename Iterator, typename End, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Count, Predicate>
next(Range<Iterator, End, Count, Predicate> const& x) {
  return Range<Iterator, End, Count, Predicate>::make(advance(getBegin(x)), getEnd(x), decrementCount(getCount(x)), getPredicate(x));
}



template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeAdvance : std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag> {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeDifference : std::is_convertible<IteratorCategory<Iterator>, std::random_access_iterator_tag> {};

template<typename T>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd : std::false_type {};

template<typename Iterator, typename Count>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, Present, Count, NotPresent>> : std::true_type {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeEnd<Range<Iterator, NotPresent, Present, NotPresent>> : ConstantTimeAdvance<Iterator> {};

template<typename T>
struct CanAddEndInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and either a count or a predicate is present to terminate the loop.
template<typename Iterator, typename Count, typename Predicate>
struct CanAddEndInLinearTime<Range<Iterator, NotPresent, Count, Predicate>> :
  std::integral_constant<bool,
   !ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value &&
   std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
  ( !std::is_same<Count, NotPresent>::value || !std::is_same<Predicate, NotPresent>::value) >
{};


// Already present
template<typename Iterator, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count, Predicate>
addConstantTimeEnd(Range<Iterator, Present, Count, Predicate> const& x) {
  return x;
}

// Can add in constant time (ends up that Predicate = NotPresent)
template<typename Iterator, typename Count, typename Predicate>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, Present, Count, Predicate>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x) {
  auto tmp = x.begin;
  std::advance(tmp, x.count);
  return addEnd(x, tmp);
}

// Can't add in constant time
template<typename Iterator, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, NotPresent, Count, Predicate>>::type
addConstantTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x) {
  return x;
}


template<typename Iterator, typename Count, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, Present, Count, Predicate>
addLinearTimeEnd(Range<Iterator, Present, Count, Predicate> const& x) {
  return x;
}

template<typename Iterator, typename Count, typename Predicate>
typename std::enable_if<ConstantTimeEnd<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, Present, Count, Predicate>>::type
addLinearTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x) {
  return addConstantTimeEnd(x);
}

template<typename Iterator, typename Count, typename Predicate>
typename std::enable_if<CanAddEndInLinearTime<Range<Iterator, NotPresent, Count, Predicate>>::value, Range<Iterator, Present, Present, NotPresent>>::type
addLinearTimeEnd(Range<Iterator, NotPresent, Count, Predicate> const& x) {
  auto tmp = x;
  CountType<Iterator> count = 0;
  while(!isEmpty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return addCount(addEnd(removePredicate(x), getBegin(tmp)), count);
}








template<typename T>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount : std::false_type {};

template<typename Iterator, typename End>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount<Range<Iterator, End, Present, NotPresent>> : std::true_type {};

template<typename Iterator>
struct TYPE_HIDDEN_VISIBILITY ConstantTimeCount<Range<Iterator, Present, NotPresent, NotPresent>> : ConstantTimeDifference<Iterator> {};

template<typename T>
struct CanAddCountInLinearTime {};

// Can add a count in linear time if we can't add it in constant time, the iteration is repeatable
// and either a count or a predicate is present to terminate the loop.
template<typename Iterator, typename End, typename Predicate>
struct CanAddCountInLinearTime<Range<Iterator, End, NotPresent, Predicate>> :
  std::integral_constant<bool,
  !ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value &&
  std::is_convertible<IteratorCategory<Iterator>, std::forward_iterator_tag>::value &&
  (!std::is_same<End, NotPresent>::value || !std::is_same<Predicate, NotPresent>::value) >
{};


// Already present
template<typename Iterator, typename End, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present, Predicate>
addConstantTimeCount(Range<Iterator, End, Present, Predicate> const& x) {
  return x;
}

// Can add in constant time (ends up that Predicate = NotPresent)
template<typename Iterator, typename End, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, End, Present, Predicate>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x) {
  return addCount(x, std::distance(x.begin, x.end));
}

// Can't add in constant time
template<typename Iterator, typename End, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<!ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, End, NotPresent, Predicate>>::type
addConstantTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x) {
  return x;
}

template<typename Iterator, typename End, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN Range<Iterator, End, Present, Predicate>
addLinearTimeCount(Range<Iterator, End, Present, Predicate> const& x) {
  return x;
}

template<typename Iterator, typename End, typename Predicate>
constexpr ALWAYS_INLINE_HIDDEN typename std::enable_if<ConstantTimeCount<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, End, Present, Predicate>>::type
addLinearTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x) {
  return addConstantTimeCount(x);
}

template<typename Iterator, typename End, typename Predicate>
ALWAYS_INLINE_HIDDEN typename std::enable_if<CanAddCountInLinearTime<Range<Iterator, End, NotPresent, Predicate>>::value, Range<Iterator, Present, Present, Predicate>>::type
addLinearTimeCount(Range<Iterator, End, NotPresent, Predicate> const& x) {
  auto tmp = x;
  CountType<Iterator> count = 0;
  while(!isEmpty(tmp)) {
    tmp = next(tmp);
    ++count;
  }
  return addCount(addEnd(x, getBegin(tmp)), count);
}

template<typename T>
ALWAYS_INLINE_HIDDEN auto addLinearTimeEndAndCount(T const& x) -> decltype(addLinearTimeCount(addLinearTimeEnd(x))) {
  return addLinearTimeCount(addLinearTimeEnd(x));
}

// Split
// Reverse
// Skip ... toughie for non-divisible ranges

} // namespace range2

#endif
