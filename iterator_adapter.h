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


namespace range2 {

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

template<InputIterator I>
constexpr ALWAYS_INLINE_HIDDEN I forwardByN(I x, DifferenceType<I> n) {
  // n >= 0
  return n ? forwardByN(++x, n-1) : x;
}

template<BidirectionalIterator I>
constexpr ALWAYS_INLINE_HIDDEN I backwardByN(I x, DifferenceType<I> n) {
  // n <= 0
  return n ? backwardByN(--x, n+1) : x;
}

template<InputIterator I>
constexpr I advance(I x, DifferenceType<I> n, std::input_iterator_tag) {
  // n >= 0
  return forwardByN(x, n);
}

template<BidirectionalIterator I>
constexpr I advance(I x, DifferenceType<I> n, std::bidirectional_iterator_tag) {
  return n < 0 ? backwardByN(x, n) : forwardByN(x, n);
}

template<RandomAccessIterator I>
constexpr I advance(I x, DifferenceType<I> n, std::random_access_iterator_tag) {
  return x + n;
}

template<InputIterator I>
constexpr I advance(I x, DifferenceType<I> n) {
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

  constexpr ALWAYS_INLINE_HIDDEN
  reference operator*() const { return deref(basis); }

  constexpr ALWAYS_INLINE_HIDDEN
  pointer operator->() const { return &(deref(basis)); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  reference deref(iterator const& x) { return *x; }

  friend ALWAYS_INLINE_HIDDEN
  iterator successor(iterator const& x) { return {successor(x.basis)}; }

  ALWAYS_INLINE_HIDDEN
  iterator& operator++() {
    basis = successor(basis);
    return *this;
  }

  ALWAYS_INLINE_HIDDEN
  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }

  friend ALWAYS_INLINE_HIDDEN
  bool operator==(const iterator& x, const iterator& y) { return state(x) == state(y); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator!=(const iterator& x, const iterator& y) { return !(x == y); }

  // for bidirectional iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
  iterator predecessor(iterator const& x) { return {predecessor(x.basis)}; }

  ALWAYS_INLINE_HIDDEN
  iterator& operator--() {
    basis = predecessor(basis);
    return *this;
  }

  ALWAYS_INLINE_HIDDEN
  iterator operator--(int) {
    iterator tmp = *this;
    --*this;
    return tmp;
  }

  // for random access iterator

  ALWAYS_INLINE_HIDDEN
  iterator& operator+=(difference_type i) {
    basis = offset(basis, i);
    return *this;
  }

  ALWAYS_INLINE_HIDDEN
  iterator& operator-=(difference_type i) {
    *this += -i;
    return *this;
  }

  ALWAYS_INLINE_HIDDEN
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

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator>(const iterator& x, const iterator& y) { return y < x; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator<=(const iterator& x, const iterator& y) { return !(y < x); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  bool operator>=(const iterator& x, const iterator& y) { return !(x < y); }
};

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
  iterator_basis offset(iterator_basis x, difference_type i) { return {x.position += i}; }

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
  reverse_iterator_basis offset(reverse_iterator_basis x, difference_type i) { return {x.position -= i}; }

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

template<typename I>
struct TYPE_HIDDEN_VISIBILITY reverse_iterator_impl {
  typedef iterator<reverse_iterator_basis<I>> type;
};

template<typename I>
struct TYPE_HIDDEN_VISIBILITY reverse_iterator_impl<iterator<reverse_iterator_basis<I>>>{
  typedef iterator<iterator_basis<I>> type;
};

template<BidirectionalIterator I>
using reverse_iterator = typename reverse_iterator_impl<I>::type;

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
  skip_iterator_basis successor(skip_iterator_basis x) { return advance(x.position, N); }

  friend constexpr ALWAYS_INLINE_HIDDEN
  skip_iterator_basis predecessor(skip_iterator_basis x) { return advance(x.position, -N); }

  // for random access iterator
  friend constexpr ALWAYS_INLINE_HIDDEN
  skip_iterator_basis offset(skip_iterator_basis x, difference_type i) { return {x.position += i * N}; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  difference_type difference(skip_iterator_basis const& x, skip_iterator_basis const& y) { return std::distance(x.position, y.position) / N; }

  friend constexpr ALWAYS_INLINE_HIDDEN
  state_type state(skip_iterator_basis const& x) { return x.position; }
};

template<InputIterator I, DifferenceType<I> N>
struct TYPE_HIDDEN_VISIBILITY SpecialisedSink<skip_iterator_basis<I, N>> : std::true_type {};

template<typename... T, InputIterator I, DifferenceType<I> N>
ALWAYS_INLINE_HIDDEN auto sink(skip_iterator_basis<I, N> const& x, T&&... y) -> decltype( sink(state(x), std::forward<T>(y)...) ) {
  return sink(state(x), std::forward<T>(y)...);
}


template<InputIterator I, DifferenceType<I> N>
struct TYPE_HIDDEN_VISIBILITY SkipIterator_Impl {
  typedef iterator<skip_iterator_basis<I, N>> type;
};

template<InputIterator I, DifferenceType<I> N, DifferenceType<I> M>
struct TYPE_HIDDEN_VISIBILITY SkipIterator_Impl<iterator<skip_iterator_basis<I, M>>, N> {
  typedef iterator<skip_iterator_basis<I, N * M>> type;
};


template<InputIterator I, DifferenceType<I> N>
using SkipIterator = typename SkipIterator_Impl<I, N>::type;

// TODO calculus of interacting skip & reverse ...

} // namespace range2

#endif
