#ifndef INCLUDED_ITERATOR_ADAPTER
#define INCLUDED_ITERATOR_ADAPTER

#ifndef INCLUDED_ITERATOR
#define INCLUDED_ITERATOR
#include <iterator>
#endif

#ifndef INCLUDED_TYPE_TRAITS
#define INCLUDED_TYPE_TRAITS
#include <type_traits>
#endif

#ifndef INCLUDED_COMPILER_SPECIFICS
#include "compiler_specifics.h"
#endif

#ifndef INCLUDED_COMPARISONS
#include "comparisons.h"
#endif

namespace range2 {

//
// Note that the code for iterator adaption was taken from
// https://github.com/psoberoi/stepanov-conversations-course
// which contains the source code from the Programming conversations course led by
// Alex Stepanov & Param Oberoi.
// See also http://www.youtube.com/user/A9Videos
// The code itself was adapted by taking out state modification functions into constexpr
// friend functions where possible.

#define InputIterator typename
#define IteratorBasis typename
#define BidirectionalIterator typename
#define RandomAccessIterator typename


template <InputIterator I>
using IteratorCategory = typename std::iterator_traits<I>::iterator_category;

template <InputIterator I>
using ValueType = typename std::iterator_traits<I>::value_type;

template <InputIterator I>
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <InputIterator I>
using Pointer = typename std::iterator_traits<I>::pointer;

template <InputIterator I>
using Reference = typename std::iterator_traits<I>::reference;


// Machinery to enable constexpr advance where possible
// Note that in C++14 std::advance is constexpr anyhow.
//
template<InputIterator I>
ALWAYS_INLINE_HIDDEN
I forwardByN(I x, DifferenceType<I> n) {
  // n >= 0
  while (n) {
    ++x, --n;
  }
  return x;
}

template<BidirectionalIterator I>
ALWAYS_INLINE_HIDDEN
I backwardByN(I x, DifferenceType<I> n) {
  // n <= 0
  while (n) {
    --x, ++n;
  }
  return x;
}

template<InputIterator I>
ALWAYS_INLINE_HIDDEN
I advance(I x, DifferenceType<I> n, std::input_iterator_tag) {
  // n >= 0
  return forwardByN(x, n);
}

template<BidirectionalIterator I>
ALWAYS_INLINE_HIDDEN
I advance(I x, DifferenceType<I> n, std::bidirectional_iterator_tag) {
  return n < 0 ? backwardByN(x, n) : forwardByN(x, n);
}

template<RandomAccessIterator I>
constexpr ALWAYS_INLINE_HIDDEN
I advance(I x, DifferenceType<I> n, std::random_access_iterator_tag) {
  return x + n;
}

template<InputIterator I>
constexpr ALWAYS_INLINE_HIDDEN
I advance(I x, DifferenceType<I> n) {
  return advance(x, n, IteratorCategory<I>());
}


template<typename Iterator, typename Enable=void>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink : std::false_type {};

template<typename T, typename I>
ALWAYS_INLINE_HIDDEN typename std::enable_if<!SpecialisedSink<I>::value, void>::type
sink(I const& x, T&& y) {
  static_assert(std::is_convertible<T, ValueType<I>>::value, "Value to sink must be convertible to value type of Iterator");
  *x = std::forward<T>(y);
};

// Concept IteratorBasis
// - semiregular
// - state_type, value_type, reference, pointer, iterator_category
// - state_type is regular
// - deref() -> reference
// - successor(IteratorBasis) -> IteratorBasis
// - state() -> state_type
// For bidirectional iterators:
// - predecessor(IteratorBasis) -> IteratorBasis
// For random access iterators:
// - difference_type
// - offset(IteratorBasis, difference_type) -> IteratorBasis
// - difference(IteratorBasis, IteratorBasis) -> difference_type
// For output iterators:
// - sink(IteratorBasis, ValueType) -> void

template <IteratorBasis B>
struct TYPE_DEFAULT_VISIBILITY iterator {
  typedef typename B::state_type state_type;
  typedef typename B::value_type value_type;
  typedef typename B::difference_type difference_type;
  typedef typename B::reference reference;
  typedef typename B::pointer pointer;
  typedef typename B::iterator_category iterator_category;

  B basis;

  friend constexpr ALWAYS_INLINE_HIDDEN
  state_type state(iterator const& x) { return state(x.basis); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  reference operator*(iterator const& x) { return deref(x.basis); }

  // Must be a non-static member function
  constexpr ALWAYS_INLINE_HIDDEN
  pointer operator->() const { return &**this; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  reference deref(iterator const& x) { return *x; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator successor(iterator const& x) { return {successor(x.basis)}; }

  friend ALWAYS_INLINE_HIDDEN
  iterator& operator++(iterator& x) {
    x.basis = successor(x.basis);
    return x;
  }

  friend ALWAYS_INLINE_HIDDEN
  iterator operator++(iterator& x, int) {
    iterator tmp = x;
    ++x;
    return tmp;
  }

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator==(const iterator& x, const iterator& y) { return state(x) == state(y); }

  // for bidirectional iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator predecessor(iterator const& x) { return {predecessor(x.basis)}; }

  friend ALWAYS_INLINE_HIDDEN
  iterator& operator--(iterator& x) {
    x.basis = predecessor(x.basis);
    return x;
  }

  friend ALWAYS_INLINE_HIDDEN
  iterator operator--(iterator& x, int) {
    iterator tmp = x;
    --x;
    return tmp;
  }

  // for random access iterator

  friend ALWAYS_INLINE_HIDDEN
  iterator& operator+=(iterator& x, difference_type i) {
    x.basis = offset(x.basis, i);
    return x;
  }

  friend ALWAYS_INLINE_HIDDEN
  iterator& operator-=(iterator& x, difference_type i) {
    return x += -i;
  }

  // Must be a non-static member function
  constexpr ALWAYS_INLINE_HIDDEN
  reference operator[](difference_type i) const { return *(*this + i); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  difference_type operator-(const iterator& x, const iterator& y) { return difference(x.basis, y.basis); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator operator-(iterator const& x, difference_type i) { return {offset(x.basis, -i)}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator operator-(difference_type i, iterator const& x) { return {offset(x.basis, -i)}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator operator+(iterator const& x, difference_type i) { return {offset(x.basis, i)}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator operator+(difference_type i, iterator const& x) { return {offset(x.basis, i)}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator<(const iterator& x, const iterator& y) { return x - y < 0; }
};

template <IteratorBasis B>
struct GenerateDerivedComparisonOperations<iterator<B>> : std::true_type {};


template<IteratorBasis I>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink<iterator<I>> : std::true_type {};

template<typename... T, IteratorBasis I>
ALWAYS_INLINE_HIDDEN auto sink(iterator<I> const& x, T&&... y) -> decltype( sink(x.basis, std::forward<T>(y)...) ) {
  return sink(x.basis, std::forward<T>(y)...);
}


template <InputIterator I>
struct TYPE_DEFAULT_VISIBILITY iterator_basis {
  typedef I state_type;
  state_type position;
  typedef ValueType<I> value_type;
  typedef Reference<I> reference;
  typedef Pointer<I> pointer;
  typedef DifferenceType<I> difference_type;
  typedef IteratorCategory<I> iterator_category;

  friend constexpr ALWAYS_INLINE_HIDDEN
  reference deref(iterator_basis const& x) { return *x.position; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator_basis successor(iterator_basis x) { return {++x.position}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator_basis predecessor(iterator_basis x) { return {--x.position}; }

  // for random access iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator_basis offset(iterator_basis x, difference_type i) { return {x.position + i}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  difference_type difference(iterator_basis const& x, iterator_basis const& y) { return std::distance(y.position, x.position); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  state_type state(iterator_basis const& x) { return x.position; }
};


template<InputIterator I>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink<iterator_basis<I>> : std::true_type {};

template<typename... T, InputIterator I>
ALWAYS_INLINE_HIDDEN auto sink(iterator_basis<I> const& x, T&&... y) -> decltype( sink(state(x), std::forward<T>(y)...) ) {
  return sink(state(x), std::forward<T>(y)...);
}

template <BidirectionalIterator I>
struct TYPE_DEFAULT_VISIBILITY reverse_iterator_basis {
  typedef I state_type;
  state_type position;
  typedef ValueType<I> value_type;
  typedef Reference<I> reference;
  typedef Pointer<I> pointer;
  typedef DifferenceType<I> difference_type;
  typedef IteratorCategory<I> iterator_category;

  // Take a copy of the iterator to be able to use -- inline
  friend constexpr ALWAYS_INLINE_HIDDEN
  reference deref(reverse_iterator_basis x) { return *(--x.position); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  reverse_iterator_basis successor(reverse_iterator_basis x) { return {--x.position}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  reverse_iterator_basis predecessor(reverse_iterator_basis x) { return {++x.position}; }

  // for random access iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
    reverse_iterator_basis offset(reverse_iterator_basis x, difference_type i) { return {x.position - i}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  difference_type difference(reverse_iterator_basis const& x, reverse_iterator_basis const& y) { return std::distance(x.position, y.position); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  state_type state(reverse_iterator_basis const& x) { return x.position; }
};


template<BidirectionalIterator I>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink<reverse_iterator_basis<I>> : std::true_type {};

template<typename... T, BidirectionalIterator I>
ALWAYS_INLINE_HIDDEN auto sink(reverse_iterator_basis<I> const& x, T&&... y) -> decltype( sink(state(successor(x)), std::forward<T>(y)...) ) {
  // Note the need to perform successor(x) to get the actual iterator we store to.
  return sink(state(successor(x)), std::forward<T>(y)...);
}


template <InputIterator I, DifferenceType<I> N>
struct TYPE_DEFAULT_VISIBILITY skip_iterator_basis {
  typedef I state_type;
  state_type position;
  typedef ValueType<I> value_type;
  typedef Reference<I> reference;
  typedef Pointer<I> pointer;
  typedef DifferenceType<I> difference_type;
  typedef IteratorCategory<I> iterator_category;

  friend constexpr ALWAYS_INLINE_HIDDEN
  reference deref(skip_iterator_basis const& x) { return *x.position; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  skip_iterator_basis successor(skip_iterator_basis x) { return {advance(x.position, N)}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  skip_iterator_basis predecessor(skip_iterator_basis x) { return {advance(x.position, -N)}; }

  // for random access iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
  skip_iterator_basis offset(skip_iterator_basis x, difference_type i) { return {x.position + i * N}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  difference_type difference(skip_iterator_basis const& x, skip_iterator_basis const& y) { return std::distance(y.position, x.position) / N; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  state_type state(skip_iterator_basis const& x) { return x.position; }

  // If you want a reverse stride then explicitly specify a positive stride and then perform a reverse.
  // This really only make sense at the Range level which knows about the limit iterators.
  static_assert(N>0, "Stride must be greater than 0");
};


template<InputIterator I, DifferenceType<I> N>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink<skip_iterator_basis<I, N>> : std::true_type {};

template<typename... T, InputIterator I, DifferenceType<I> N>
ALWAYS_INLINE_HIDDEN auto sink(skip_iterator_basis<I, N> const& x, T&&... y) -> decltype( sink(state(x), std::forward<T>(y)...) ) {
  return sink(state(x), std::forward<T>(y)...);
}

template<InputIterator I>
struct TYPE_HIDDEN_VISIBILITY iterator_impl {
  typedef iterator<iterator_basis<I>> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(I x) {
    return {{x}};
  }
};

template<typename Basis>
struct TYPE_HIDDEN_VISIBILITY iterator_impl<iterator<Basis>> {
  typedef iterator<Basis> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(iterator<Basis> x) {
    return x;
  }
};

template<InputIterator I>
using wrapped_iterator = typename iterator_impl<I>::type;

template<InputIterator I>
constexpr ALWAYS_INLINE_HIDDEN wrapped_iterator<I> make_iterator(I x) {
  return iterator_impl<I>::apply(x);
}

template<BidirectionalIterator I>
struct TYPE_HIDDEN_VISIBILITY reverse_iterator_impl {
  typedef iterator<reverse_iterator_basis<I>> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(I x) {
    return {{x}};
  }
};

template<BidirectionalIterator I>
struct TYPE_HIDDEN_VISIBILITY reverse_iterator_impl<iterator<reverse_iterator_basis<I>>>{
  typedef iterator<iterator_basis<I>> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(iterator<reverse_iterator_basis<I>> x) {
    return make_iterator(state(x));
  }
};

template<BidirectionalIterator I>
using reverse_iterator = typename reverse_iterator_impl<I>::type;

template<BidirectionalIterator I>
constexpr ALWAYS_INLINE_HIDDEN reverse_iterator<I> make_reverse_iterator(I x) {
  return reverse_iterator_impl<I>::apply(x);
}

template<InputIterator I, DifferenceType<I> N>
struct TYPE_HIDDEN_VISIBILITY skip_iterator_impl {
  typedef iterator<skip_iterator_basis<I, N>> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(I x) {
    return {{x}};
  }
};

template<InputIterator I, DifferenceType<I> N, DifferenceType<I> M>
struct TYPE_HIDDEN_VISIBILITY skip_iterator_impl<iterator<skip_iterator_basis<I, M>>, N> {
  typedef typename skip_iterator_impl<I, N * M>::type type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(iterator<skip_iterator_basis<I, M>> x) {
    return skip_iterator_impl<I, N * M>::apply(state(x));
  }
};

template<BidirectionalIterator I, DifferenceType<I> N>
struct TYPE_HIDDEN_VISIBILITY skip_iterator_impl<iterator<reverse_iterator_basis<I>>, N>{
  typedef reverse_iterator<typename skip_iterator_impl<I, N>::type> type;

  static constexpr ALWAYS_INLINE_HIDDEN type apply(iterator<reverse_iterator_basis<I>> x) {
    return make_reverse_iterator(skip_iterator_impl<I, N>::apply(state(x)));
  }
};

template<InputIterator I, DifferenceType<I> N>
using skip_iterator = typename skip_iterator_impl<I, N>::type;

// For the function skip_iterator we can't get the DifferenceType before knowing the iterator
// but then this would mean callers have to specify 
typedef long long SkipIteratorStride;

template<SkipIteratorStride N, InputIterator I>
constexpr ALWAYS_INLINE_HIDDEN skip_iterator<I, N> make_skip_iterator(I x) {
  return skip_iterator_impl<I, N>::apply(x);
}

} // namespace range2

#endif
