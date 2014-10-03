#include "range2.h"
#include "algorithms.h"
#include "timer.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <functional>
#include <forward_list>


namespace range2 {
namespace {

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  typedef int const* PtrType;

  constexpr int count = 40;
  constexpr int arr[count] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39};
  constexpr PtrType begin = &arr[0];
  constexpr PtrType end = &arr[0] + count;

  const std::forward_list<int> slist(begin, end);

  // Test Range construction
  constexpr auto r00 = make_range(begin, NotPresent{}, NotPresent{});
  constexpr auto r01 = make_range(begin, NotPresent{}, count);
  constexpr auto r10 = make_range(begin, end, NotPresent{});
  constexpr auto r11 = make_range(begin, end, count);

  constexpr pair<int, int> pairArray[2] = {make_pair(0,0), make_pair(0,1)};

#define RELATIONAL_TESTS(Lookup, Assert)\
    Assert(Lookup(lesser) == Lookup(lesser));\
    Assert(!(Lookup(lesser) != Lookup(lesser)));\
\
    Assert(Lookup(greater) == Lookup(greater));\
    Assert(!(Lookup(greater) != Lookup(greater)));\
\
    Assert(Lookup(lesser) != Lookup(greater));\
    Assert(!(Lookup(lesser) == Lookup(greater)));\
\
    Assert(!(Lookup(lesser) < Lookup(lesser)));\
    Assert(Lookup(lesser) <= Lookup(lesser));\
    Assert(Lookup(lesser) >= Lookup(lesser));\
    Assert(!(Lookup(lesser) > Lookup(lesser)));\
\
    Assert(!(Lookup(greater) < Lookup(greater)));\
    Assert(Lookup(greater) <= Lookup(greater));\
    Assert(Lookup(greater) >= Lookup(greater));\
    Assert(!(Lookup(greater) > Lookup(greater)));\
\
    Assert(Lookup(lesser) < Lookup(greater));\
    Assert(Lookup(lesser) <= Lookup(greater));\
    Assert(!(Lookup(lesser) > Lookup(greater)));\
    Assert(!(Lookup(lesser) >= Lookup(greater)));\
\
    Assert(!(Lookup(greater) < Lookup(lesser)));\
    Assert(!(Lookup(greater) <= Lookup(lesser)));\
    Assert(Lookup(greater) > Lookup(lesser));\
    Assert(Lookup(greater) >= Lookup(lesser));\

#define PAIR_ARRAY_LOOKUP(x) pairArray[x]
#define TEST_ASSERTION TEST_ASSERT

  template<int lesser, int greater>
  constexpr bool testPairRelations() {
    RELATIONAL_TESTS(PAIR_ARRAY_LOOKUP, TEST_ASSERTION)
    return true;
  }

#undef PAIR_ARRAY_LOOKUP
#define RUNTIME_ASSERTION assert
#define NO_LOOKUP(x) x

  void testPair() {
    testPairRelations<0, 1>();
  }

  template<typename T>
  void testRelationalOperators(T const& lesser, T const& greater) {
    RELATIONAL_TESTS(NO_LOOKUP, RUNTIME_ASSERTION)
  }

#undef NO_LOOKUP
#undef RUNTIME_ASSERTION

  constexpr triple<int, int, int> tripleArray[4] = {make_triple(0,0,0), make_triple(1,0,0), make_triple(0,1,0), make_triple(0,0,1)};

#define TRIPLE_ARRAY_LOOKUP(x) tripleArray[x]

  template<int lesser, int greater>
  constexpr bool testTripleRelations() {
    RELATIONAL_TESTS(TRIPLE_ARRAY_LOOKUP, TEST_ASSERTION)
    return true;
  }

#undef TRIPLE_ARRAY_LOOKUP
#undef TEST_ASSERTION
#undef RELATIONAL_TESTS

  void testTriple() {
    testTripleRelations<0,1>();
    testTripleRelations<0,2>();
    testTripleRelations<0,3>();
    testTripleRelations<2,1>();
    testTripleRelations<3,2>();
    testTripleRelations<3,2>();
  }

  void testPresentNotPresent() {
    TEST_ASSERT(Present{} == Present{});
    TEST_ASSERT(!(Present{} != Present{}));
    TEST_ASSERT(!(Present{} < Present{}));

    TEST_ASSERT(NotPresent{} == NotPresent{});
    TEST_ASSERT(!(NotPresent{} != NotPresent{}));
    TEST_ASSERT(!(NotPresent{} < NotPresent{}));
  }

  void testGetBegin() {
    TEST_ASSERT(begin == get_begin(r00));
    TEST_ASSERT(begin == get_begin(r01));
    TEST_ASSERT(begin == get_begin(r10));
    TEST_ASSERT(begin == get_begin(r11));

    // Test non-const reference path
    auto tmp = make_range(begin, NotPresent{}, NotPresent{});
    PtrType& begin2 = get_begin(tmp);
    assert(begin2 == begin);
  }

  void testGetEnd() {
    TEST_ASSERT(end == get_end(r10));
    TEST_ASSERT(end == get_end(r11));

    // Test non-const reference path
    auto tmp = make_range(begin, end, NotPresent{});
    PtrType& end2 = get_end(tmp);
    assert(end2 == end);

    NotPresent tmp2 = get_end(r00);
    (void)tmp2;

    NotPresent tmp3 = get_end(r01);
    (void)tmp3;
  }

  void testGetCount() {
    TEST_ASSERT(count == get_count(r01));
    TEST_ASSERT(count == get_count(r11));

    // Test non-const reference path
    auto tmp = make_range(begin, end, count);
    DifferenceType<PtrType>& count2 = get_count(tmp);
    assert(count2 == count);

    NotPresent tmp2 = get_count(r00);
    (void)tmp2;

    NotPresent tmp3 = get_count(r10);
    (void)tmp3;
  }

  void testAddEnd() {
    TEST_ASSERT(end == get_end(add_end(r00, end)));
    TEST_ASSERT(end == get_end(add_end(r01, end)));
    TEST_ASSERT(end == get_end(add_end(r10, end)));
    TEST_ASSERT(end == get_end(add_end(r11, end)));
  }

  void testAddCount() {
    constexpr int x = 123;

    TEST_ASSERT(x == get_count(add_count(r00, x)));
    TEST_ASSERT(x == get_count(add_count(r01, x)));
    TEST_ASSERT(x == get_count(add_count(r10, x)));
    TEST_ASSERT(x == get_count(add_count(r11, x)));
  }

  void testRangeRelationals() {
    // tests first clause of <
    testRelationalOperators(r00, successor(r00));
    testRelationalOperators(r01, successor(r01));
    testRelationalOperators(r10, successor(r10));
    testRelationalOperators(r11, successor(r11));

    {
      auto tmp = r01;
      tmp = add_count(tmp, get_count(tmp) - 1);
      testRelationalOperators(tmp, r01);
    }

    {
      auto tmp = r10;
      auto i = get_end(tmp);
      --i;
      tmp = add_end(tmp, i);
      testRelationalOperators(tmp, r10);
    }

    {
      auto tmp = r11;
      tmp = add_count(tmp, get_count(tmp) - 1);
      auto i = get_end(tmp);
      --i;
      tmp = add_end(tmp, i);
      testRelationalOperators(tmp, r11);
    }

    // Check the third clause of <, where Count differs
    {
      auto tmp = r11;
      tmp = add_count(tmp, get_count(tmp) - 1);
      testRelationalOperators(tmp, r11);
    }
  }

  void testRemoveEnd() {
    {
      auto tmp = remove_end(r10);
      NotPresent tmp2 = get_end(tmp);
      (void)tmp2;
    }

    {
      auto tmp = remove_end(r11);
      NotPresent tmp2 = get_end(tmp);
      (void)tmp2;
    }
  }

  void testRemoveCount() {
    {
      auto tmp = remove_count(r01);
      NotPresent tmp2 = get_count(tmp);
      (void)tmp2;
    }

    {
      auto tmp = remove_count(r11);
      NotPresent tmp2 = get_count(tmp);
      (void)tmp2;
    }
  }

  template<typename T>
  void testAddConstantTimeEndImpl(T x) {
      auto tmp = add_constant_time_end(x);
      assert(get_begin(x) == get_begin(tmp));
      assert(end == get_end(tmp));
      assert(get_count(x) == get_count(tmp));
  }

  template<typename T>
  void testAddConstantTimeEndNotPossibleImpl(T x) {
      auto tmp = add_constant_time_end(x);
      assert(get_begin(x) == get_begin(tmp));
      NotPresent tmp2 = get_end(tmp);
      (void)tmp2;
      assert(get_count(x) == get_count(tmp));
  }

  void testAddConstantTimeEnd() {
    testAddConstantTimeEndImpl(r01);
    testAddConstantTimeEndImpl(r10);
    testAddConstantTimeEndImpl(r11);

    testAddConstantTimeEndNotPossibleImpl(r00);
    testAddConstantTimeEndNotPossibleImpl(make_range(slist.begin(), NotPresent{}, count));
  }

  template<typename T>
  void testAddConstantTimeCountImpl(T x) {
    auto tmp = add_constant_time_count(x);
    assert(get_begin(x) == get_begin(tmp));
    assert(get_end(x) == get_end(tmp));
    assert(count == get_count(tmp));
  }

  template<typename T>
  void testAddConstantTimeCountNotPossibleImpl(T x) {
      auto tmp = add_constant_time_count(x);
      assert(get_begin(x) == get_begin(tmp));
      assert(get_end(x) == get_end(tmp));
      NotPresent tmp2 = get_count(tmp);
      (void)tmp2;
  }

  void testAddConstantTimeCount() {
    testAddConstantTimeCountImpl(r10);
    testAddConstantTimeCountImpl(r01);
    testAddConstantTimeCountImpl(r11);

    testAddConstantTimeCountNotPossibleImpl(r00);
    testAddConstantTimeCountNotPossibleImpl(make_range(slist.begin(), slist.end(), NotPresent{}));
  }

  template<typename T>
  void testAddLinearTimeEndImpl(T x) {
    auto tmp = add_linear_time_end(x);
    assert(get_begin(x) == get_begin(tmp));
    assert(end == get_end(tmp));
    assert(count == get_count(tmp));
  }

  template<typename T>
  void testAddLinearTimeEndNoCountCheckImpl(T x) {
    auto tmp = add_linear_time_end(x);
    assert(get_begin(x) == get_begin(tmp));
    assert(end == get_end(tmp));
  }

  void testAddLinearTimeEnd() {
    testAddLinearTimeEndImpl(r01);
    testAddLinearTimeEndImpl(r11);
    testAddLinearTimeEndImpl(r11);

    {
      auto tmp = add_linear_time_end(make_range(slist.begin(), NotPresent{}, count));
      assert(slist.end() == get_end(tmp));
      assert(count == get_count(tmp));
    }
  }

  template<typename T>
  void testAddLinearTimeCountImpl(T x) {
    auto tmp = add_linear_time_count(x);
    assert(count == get_count(tmp));
    assert(end == get_end(tmp));
  }

  template<typename T>
  void testAddLinearTimeCountNoEndCheckImpl(T x) {
    auto tmp = add_linear_time_count(x);
    assert(count == get_count(tmp));
  }

  void testAddLinearTimeCount() {
    // Can't call for r00 as this don't have a termiantion condition.
    testAddLinearTimeCountImpl(r10);
    testAddLinearTimeCountImpl(r11);

    {
      auto tmp = add_linear_time_count(make_range(slist.begin(), slist.end(), NotPresent{}));
      assert(count == get_count(tmp));
      assert(slist.end() == get_end(tmp));
    }
  }

  void testIsEmpty() {
    assert(!is_empty(r00));
    assert(!is_empty(r01));
    assert(!is_empty(r10));
    assert(!is_empty(r11));
    
    {
      auto tmp = make_range(begin, begin, NotPresent{});
      assert(is_empty(tmp));
    }

    {
      auto tmp = make_range(begin, NotPresent{}, 0);
      assert(is_empty(tmp));
    }

    {
      // Demonstrates that count is used in preference to begin/end pair
      auto tmp = make_range(begin, end, 0);
      assert(is_empty(tmp));
    }
  }

  template<typename T>
  void testBoundedRange(T x) {
    int i = 0;
    while (!is_empty(x)) {
      assert(i == *get_begin(x));
      ++i;
      x = successor(x);
    }
    assert(count == i);
  }

  void testSuccessor() {
    testBoundedRange(make_range(begin, end, NotPresent{}));
    testBoundedRange(make_range(begin, NotPresent{}, count));
    testBoundedRange(make_range(begin, end, count));
  }

  template<typename R0, typename R1, typename R2>
  void checkBoundedness(R0 const&, pair<R1, R2> const&) {
    static_assert(IsAFiniteRange<R1>::value, "Must be a bounded range");
    static_assert(IsAFiniteRange<R0>::value == IsAFiniteRange<R2>::value, "Boundedness must be equal");
  }

  void testSplitAt() {
    // For r00, r01, r10 & r11 need to check the case of providing
    // a middle, an end, and both a middle and an end. Note that providing just an end forces split_at
    // to calculate the middle too.
    constexpr int LHSCount = 9;

    {
      auto tmp = split_at(r00, begin + LHSCount, NotPresent{});
      checkBoundedness(r00, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      NotPresent np0 = get_count(tmp.m0);
      (void)np0;

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np1 = get_end(tmp.m1);
      (void)np1;
      NotPresent np2 = get_count(tmp.m1);
      (void)np2;
    }

    {
      auto tmp = split_at(r00, NotPresent{}, LHSCount);
      checkBoundedness(r00, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np0 = get_end(tmp.m1);
      (void)np0;
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r00, begin + LHSCount, LHSCount);
      checkBoundedness(r00, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np0 = get_end(tmp.m1);
      (void)np0;
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r01, begin + LHSCount, NotPresent{});
      checkBoundedness(r01, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      // count has to be calculated anyhow
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np0 = get_end(tmp.m1);
      (void)np0;
      assert((count-LHSCount) == get_count(tmp.m1));
    }

    {
      auto tmp = split_at(r01, NotPresent{}, LHSCount);
      checkBoundedness(r01, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np0 = get_end(tmp.m1);
      (void)np0;
      assert((count-LHSCount) == get_count(tmp.m1));
    }

    {
      auto tmp = split_at(r01, begin + LHSCount, LHSCount);
      checkBoundedness(r01, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      NotPresent np0 = get_end(tmp.m1);
      (void)np0;
      assert((count-LHSCount) == get_count(tmp.m1));
    }

    {
      auto tmp = split_at(r10, begin + LHSCount, NotPresent{});
      checkBoundedness(r10, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + LHSCount == get_end(tmp.m0));
      NotPresent np0 = get_count(tmp.m0);
      (void)np0;

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r10, NotPresent{}, LHSCount);
      checkBoundedness(r10, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r10, begin + LHSCount, LHSCount);
      checkBoundedness(r10, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r11, begin + LHSCount, NotPresent{});
      checkBoundedness(r11, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      NotPresent np0 = get_count(tmp.m0);
      (void)np0;

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      NotPresent np1 = get_count(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = split_at(r11, NotPresent{}, LHSCount);
      checkBoundedness(r11, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      // Needs to calculate middle anyhow
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      assert((count-LHSCount) == get_count(tmp.m1));
    }

    {
      auto tmp = split_at(r11, begin + LHSCount, LHSCount);
      checkBoundedness(r11, tmp);

      assert(!is_empty(tmp.m0));
      assert(begin == get_begin(tmp.m0));
      assert(begin + LHSCount == get_end(tmp.m0));
      assert(LHSCount == get_count(tmp.m0));

      assert(!is_empty(tmp.m1));
      assert(begin + LHSCount == get_begin(tmp.m1));
      assert(end == get_end(tmp.m1));
      assert((count-LHSCount) == get_count(tmp.m1));
    }
  }

  template<typename R0, typename R1, typename Result>
  void testJoinBoundedness(R0 const&, R1 const&, Result const&) {
    static_assert(IsAFiniteRange<R1>::value == IsAFiniteRange<Result>::value, "Error");
  }

  template<typename R>
  void testJoinImpl(R const& rng) {
    constexpr int LHSCount = 9;
    {
      auto r = make_range(begin + LHSCount, NotPresent{}, NotPresent{});
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == get_begin(tmp));
      NotPresent np0 = get_end(tmp);
      np0 = get_count(tmp);
    }

    {
      auto r = make_range(begin + LHSCount, end, NotPresent{});
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == get_begin(tmp));
      assert(end == get_end(tmp));
      NotPresent np0 = get_count(tmp);
      (void)np0;
    }

    {
      auto r = make_range(begin + LHSCount, NotPresent{}, count-LHSCount);
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == get_begin(tmp));
      assert(end == get_end(tmp));
    }

    {
      auto r = make_range(begin + LHSCount, end, count-LHSCount);
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == get_begin(tmp));
      assert(end == get_end(tmp));
    }
  }

  void testJoin() {
    testJoinImpl(r00);
    testJoinImpl(r01);
    testJoinImpl(r10);
    testJoinImpl(r11);
  }


  void testIterator() {
    auto tmp = make_iterator(begin);
    assert(0 == *tmp);
    assert(tmp == tmp);
    assert(!(tmp != tmp));

    // Test successor and equality
    auto tmp2 = successor(tmp);
    assert(1 == *tmp2);
    assert(begin+1 == state(tmp2));
    assert(1 == deref(tmp2));
    assert(tmp != tmp2);
    assert(!(tmp == tmp2));

    // Test relational operators
    assert(tmp < tmp2);
    assert(!(tmp2 < tmp));

    assert(tmp <= tmp2);
    assert(!(tmp2 <= tmp));

    assert(!(tmp > tmp2));
    assert(tmp2 > tmp);

    assert(!(tmp >= tmp2));
    assert(tmp2 >= tmp);

    // Test predecessor
    auto tmp3 = predecessor(tmp2);
    assert(tmp3 == tmp);

    // Test increment/derement
    auto tmp4 = tmp;
    ++tmp4;
    assert(tmp4 == tmp2);
    --tmp4;
    assert(tmp == tmp4);

    // test postincrement
    assert(0 == *tmp4++);
    assert(1 == *tmp4);

    // test postdecrement
    assert(1 == *tmp4--);
    assert(0 == *tmp4);

    // Test Random Access Operations

    // test operator +=
    auto tmp5 = tmp;
    assert(6 == *(tmp5 += 6));
    assert(6 == *tmp5);

    // test operator -=
    assert(4 == *(tmp5 -= 2));
    assert(4 == *tmp5);

    // test indexing operator
    assert(3 == tmp[3]);
    assert(0 == *tmp);

    // test operator+
    assert(4 == *(tmp + 4));
    assert(4 == *(4 + tmp));

    // test iterator difference
    assert(2 == ((tmp + 2) - tmp));

    // test operator-
    assert(2 == *(tmp5 - 2));
    assert(2 == *(2 - tmp5));

    //Test sink
    int arr[] = {0, 1, 2};
    auto tmp6 = make_iterator(&arr[0]);
    sink(tmp6 + 2, 55);
    assert(55 == tmp6[2]);

    // Test operator ->
    std::string arr2[] = {"aaa"};
    auto tmp7 = make_iterator(&arr2[0]);
    assert(!tmp7->empty());
  }

  void testReversedIterator() {
    auto Start = make_reverse_iterator(end);
    auto End = make_reverse_iterator(begin);

    // check difference
    assert(count == End - Start);

    //check equality and comparison
    assert(Start == Start);
    assert(End == End);
    assert(Start < End);

    //Check successor and deref
    int expected = count-1;
    while (Start != End) {
      assert(expected == *Start);
      --expected;
      ++Start;
    }
    assert(-1 == expected);

    // check predecessor
    expected = 0;
    Start = make_reverse_iterator(end);
    while (Start != End) {
      --End;
      assert(expected == *End);
      ++expected;
    }
    assert(count == expected);

    // Check state
    End = make_reverse_iterator(begin);
    assert(begin == state(End));
    assert(end == state(Start));

    // Check offset
    assert(End == Start + count);

    int arr[] = {0, 1, 2};
    // point to the end of the array
    auto tmp = make_reverse_iterator(&arr[0] + 3);
    sink(tmp, 5);
    assert(5 == arr[2]);
  }

  template<int N>
  void testSkipIteratorImpl() {
    auto Start = make_skip_iterator<N>(begin);
    auto End = make_skip_iterator<N>(end);
    assert(begin == state(Start));
    assert(end == state(End));

    // Test successor
    int expected = 0;
    while (Start != End) {
      assert(expected == *Start);
      expected += N;
      ++Start;
    }
    assert(count == expected);

    // Test predecessor
    Start = make_skip_iterator<N>(begin);
    expected = count - N;
    while (Start != End) {
      --End;
      assert(expected == *End);
      expected -= N;
    }
    assert(-N == expected);

    End = make_skip_iterator<N>(end);
    assert(Start + count/N == End);
    assert(End - count/N == Start);
    assert(End - Start == count/N);
  }

  void testSkipIterator() {
    // test the factors of 20
    testSkipIteratorImpl<1>();
    testSkipIteratorImpl<2>();
    testSkipIteratorImpl<4>();
    testSkipIteratorImpl<5>();
    testSkipIteratorImpl<10>();
    testSkipIteratorImpl<20>();
  }

  template<typename T>
  void testReverseImpl(T x) {
    auto range = reverse(x);
    int expected = count-1;
    while (!is_empty(range)) {
      assert(expected == deref(get_begin(range)));
      --expected;
      range = successor(range);
    }
    assert(-1 == expected);
  }

  void testReverse() {
    testReverseImpl(r01);
    testReverseImpl(r10);
    testReverseImpl(r11);
  }

  template<long long N, typename T>
  void testSkipImpl2(T x) {
    auto y = skip<N>(x);

    // Test successor
    int expected = 0;
    while (!is_empty(y)) {
      assert(expected == *get_begin(y));
      expected += N;
      y = successor(y);
    }
    assert(count == expected);
  }

  template<long long N>
  void testSkipImpl() {
    testSkipImpl2<N>(r01);
    testSkipImpl2<N>(r11);
  }

  void testSkip() {
    testSkipImpl<1>();
    testSkipImpl<2>();
    testSkipImpl<4>();
    testSkipImpl<5>();
    testSkipImpl<10>();
    testSkipImpl<20>();
  }

  template<int N, typename T>
  void testReverseSkipImpl2(T x) {
    int expected = count - N;
    while (!is_empty(x)) {
      assert(expected == *get_begin(x));
      x = successor(x);
      expected -= N;
    }
    assert(-N == expected);
  }

  template<int N, typename T>
  void testReverseSkipImpl1(T x) {
    testReverseSkipImpl2<N>(reverse(skip<N>(x)));
    testReverseSkipImpl2<N>(skip<N>(reverse(x)));
  }

  template<typename T>
  void testReverseSkipImpl(T x) {
    testReverseSkipImpl1<1>(x);
    testReverseSkipImpl1<2>(x);
    testReverseSkipImpl1<4>(x);
    testReverseSkipImpl1<5>(x);
    testReverseSkipImpl1<10>(x);
    testReverseSkipImpl1<20>(x);
  }

  void testReverseSkip() {
    testReverseSkipImpl(r01);
    testReverseSkipImpl(r11);
  }


  typedef unsigned long long SumType;

  template<typename T>
  SumType sumOver(T x) {
    SumType tmp = 0;
    while (!is_empty(x)) {
      tmp += *get_begin(x);
      x = successor(x);
    }
    return tmp;
  }

  template<typename T>
  SumType unrolledSumOver1(T x) {
    auto count = get_count(x);
    auto begin = get_begin(x);
    SumType tmp = 0;
    while (decltype(count){0} != count) {
      tmp += *begin++;
      --count;
    }

    return tmp;
  }

  template<typename T>
  SumType unrolledSumOver2(T x) {
    SumType tmp0 = 0;
    SumType tmp1 = 0;
    auto count = get_count(x);
    auto cBy2 = count/2;
    count -= cBy2*2;
    auto begin = get_begin(x);
    while (decltype(cBy2){0} != cBy2) {
      tmp0 += *begin;
      tmp1 += *(begin+1);
      begin += 2;
      --cBy2;
    }
    if (decltype(count){0} != count) {
      tmp0 += *begin;
    }

    return tmp0+tmp1;
  }

  template<typename T>
  SumType unrolledSumOver4(T x) {
    SumType tmp0 = 0;
    SumType tmp1 = 0;
    SumType tmp2 = 0;
    SumType tmp3 = 0;
    auto count = get_count(x);
    auto cBy4 = count/4;
    count -= cBy4*4;
    auto begin = get_begin(x);
    while (decltype(cBy4){0} != cBy4) {
      tmp0 += *begin;
      tmp1 += *(begin+1);
      tmp2 += *(begin+2);
      tmp3 += *(begin+3);
      begin += 4;
      --cBy4;
    }
    SumType tmp = tmp0 + tmp1 + tmp2 + tmp3;
    while (decltype(count){0} != count) {
      tmp += *begin++;
      --count;
    }

    return tmp;
  }

  template<typename T>
  SumType unrolledSumOver4a(T x) {
    SumType tmp0 = 0;
    SumType tmp1 = 0;
    SumType tmp2 = 0;
    SumType tmp3 = 0;
    auto count = get_count(x);
    auto cBy4 = count/4;
    count -= cBy4*4;
    auto range = make_range(get_begin(x), NotPresent{}, NotPresent{});
    while (decltype(cBy4){0} != cBy4) {
      auto r1 = successor(range);
      auto r2 = successor(r1);
      auto r3 = successor(r2);
      tmp0 += *get_begin(range);
      tmp1 += *get_begin(r1);
      tmp2 += *get_begin(r2);
      tmp3 += *get_begin(r3);
      --cBy4;
      range = successor(r3);
    }
    SumType tmp4 = 0;
    while (decltype(count){0} != count) {
      tmp4 += *get_begin(range);
      range = successor(range);
      --count;
    }

    return tmp0 + tmp1 + tmp2 + tmp3 + tmp4;
  }

  template<typename T>
  SumType unrolledSumOver8(T x) {
    SumType tmp0 = 0;
    SumType tmp1 = 0;
    SumType tmp2 = 0;
    SumType tmp3 = 0;
    SumType tmp4 = 0;
    SumType tmp5 = 0;
    SumType tmp6 = 0;
    SumType tmp7 = 0;
    auto count = get_count(x);
    auto cBy8 = count/8;
    count -= cBy8*8;
    auto begin = get_begin(x);
    while (decltype(cBy8){0} != cBy8) {
      tmp0 += *begin;
      tmp1 += *(begin+1);
      tmp2 += *(begin+2);
      tmp3 += *(begin+3);
      tmp4 += *(begin+4);
      tmp5 += *(begin+5);
      tmp6 += *(begin+6);
      tmp7 += *(begin+7);
      begin += 8;
      --cBy8;
    }
    SumType tmp = tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6 + tmp7;
    while (decltype(count){0} != count) {
      tmp += *begin++;
      --count;
    }

    return tmp;
  }


  constexpr int loopTimes = 100;

  template<typename T, typename Op>
  void performanceTestImpl(T x, char const* const description, char const* unrollDescription, Op op) {
    SumType sum = 0U;
    timer t;
    t.start();
    for (int i=0; i < loopTimes; ++i) {
      sum += op(x);
    }
    auto time = t.stop();
    std::cout << sum << ' ' << time << unrollDescription << description << std::endl;
  }

  template<typename T>
  void performanceTest(T x, char const* const description) {
    performanceTestImpl(x, description, "", [](T x) -> SumType { return sumOver(x); });
  }

  template<typename T>
  void performanceTestUnrolled(T x, char const* const description) {
    performanceTestImpl(x, description, "", [](T x) -> SumType { return sumOver(x); });
    performanceTestImpl(x, description, " Unrolled (1)", [](T x) -> SumType { return unrolledSumOver1(x); });
    performanceTestImpl(x, description, " Unrolled (2)", [](T x) -> SumType { return unrolledSumOver2(x); });
    performanceTestImpl(x, description, " Unrolled (4)", [](T x) -> SumType { return unrolledSumOver4(x); });
    performanceTestImpl(x, description, " Unrolled (4a)", [](T x) -> SumType { return unrolledSumOver4a(x); });
    performanceTestImpl(x, description, " Unrolled (8)", [](T x) -> SumType { return unrolledSumOver8(x); });
  }

  template<int LinearSearchLength, typename T>
  void performanceTestPartitionPoint(T x, char const* const description, std::vector<SumType> const& toFind) {
    timer t;
    t.start();
    SumType sum = 0;
    SumType s = 0;
    auto op = [&s](SumType y) { return s < y; };
    auto pred = make_derefop(op);
    auto toFindSize = toFind.size();
    for (int i=0; i < toFindSize; ++i) {
      s = toFind[i];
      //auto tmp = partition_point(x, pred, p);
      auto tmp = bisecting_search<T, LinearSearchLength>(x, pred, impl::Halve{});
      if (!is_empty(tmp.m1)) sum += *get_begin(tmp.m1);
    }
    auto time = t.stop();
    std::cout << "bisecting_search sum" << sum << ' ' << time << description << std::endl;
  }

  void testPerformance() {
    typedef std::vector<SumType> V;
    V v(1000000);
    std::iota(v.begin(), v.end(), 5);

    auto r0 = make_range(v.begin(), v.end(), NotPresent{});
    auto r1 = make_range(v.begin(), NotPresent{}, v.size());
    auto r2 = make_range(v.begin(), v.end(), v.size());

    auto r3 = make_range(make_iterator(v.begin()), make_iterator(v.end()), NotPresent{});
    auto r4 = make_range(make_iterator(v.begin()), NotPresent{}, v.size());
    auto r5 = make_range(make_iterator(v.begin()), make_iterator(v.end()), v.size());

    //Warm up the cache
    sumOver(r0);
    performanceTest(r0, " Bounded Range");
    performanceTest(reverse(r0), " Reverse Bounded Range");
    performanceTest(r3, " Bounded wrapped Range");
    performanceTest(reverse(r3), " Reverse Bounded wrapped Range");

    performanceTestUnrolled(r1, " Counted Range");
    performanceTestUnrolled(reverse(r1), " Reversed Counted Range");
    performanceTestUnrolled(r2, " Bounded and Counted Range");
    performanceTestUnrolled(reverse(r2), " Reversed Bounded and Counted Range");
    performanceTestUnrolled(r4, " Counted wrapped Range");
    performanceTestUnrolled(reverse(r4), " Reversed Counted wrapped Range");
    performanceTestUnrolled(r5, " Bounded and Counted wrapped Range");
    performanceTestUnrolled(reverse(r5), " Reverse Bounded and Counted wrapped Range");

    performanceTestImpl(std::cref(v), "std::accumulate", "", [](std::reference_wrapper<V const> x) -> SumType { return std::accumulate(x.get().cbegin(), x.get().cend(), SumType(0)); });

    V v2 = v;
    std::random_shuffle(v2.begin(), v2.end());

    performanceTestPartitionPoint<0>(r1, " Counted Range 0", v2);
    performanceTestPartitionPoint<8>(r1, " Counted Range 8", v2);
    performanceTestPartitionPoint<16>(r1, " Counted Range 16", v2);
    performanceTestPartitionPoint<32>(r1, " Counted Range 32", v2);
    performanceTestPartitionPoint<64>(r1, " Counted Range 64", v2);
   }

  template<typename Op>
  void forEachRangeRun(Op op) {
    // Using arr
    op(make_range(begin, end, NotPresent{}));
    op(make_range(begin, NotPresent{}, count));
    op(make_range(begin, end, count));

    op(make_range(begin, begin, NotPresent{}));
    op(make_range(begin, NotPresent{}, 0));
    op(make_range(begin, begin, 0));

    std::forward_list<int> x(begin, end);
    op(make_range(x.begin(), x.end(), NotPresent{}));
    op(make_range(x.begin(), NotPresent{}, count));
    op(make_range(x.begin(), x.end(), count));
    op(make_range(x.begin(), x.begin(), NotPresent{}));
    op(make_range(x.begin(), NotPresent{}, 0));
    op(make_range(x.begin(), x.begin(), 0));
  }

  struct TestForEachOp {

    template<typename T>
    struct Summation {
      T count;
      template<typename Iterator>
       void operator()(Iterator x) { count += *x; }
   };

    template<typename R>
    void operator()(R r) const {
      if (is_empty(r)) {
        auto tmp = for_each(r, Summation<int>{0});
        assert(tmp.m0.count == 0);
      } else {
        auto tmp = for_each(r, Summation<int>{0});
        assert(tmp.m0.count == 39*20);
      }
    }
  };

  struct TestFindIfOp {

    template<typename T>
    struct FindEqual {
      T value;
      template<typename Iterator>
      bool operator()(Iterator x) { return value == *x; }
    };

    template<typename R>
    void operator()(R x) const {
      if (is_empty(x)) {
        {
          auto tmp = find_if(x, FindEqual<int>{5});
          assert(is_empty(tmp));
        }
        {
          auto tmp = find_if(x, FindEqual<int>{-1});
          assert(is_empty(tmp));
        }
      } else {
        {
          auto tmp = find_if(x, FindEqual<int>{5});
          assert(!is_empty(tmp));
          assert(5 == *get_begin(tmp));
        }
        {
          auto tmp = find_if(x, FindEqual<int>{-1});
          assert(is_empty(tmp));
        }
      }
    }
  };

  struct TestCountIfOp {

    template<typename T>
    struct FindLessThan {
      T value;
      template<typename Iterator>
      bool operator()(Iterator x) { return *x < value; }
    };

    template<typename T>
    void operator()(T x) const {
      if (is_empty(x)) {
        {
          auto tmp = count_if(x, FindLessThan<int>{10}, 0);
          assert(tmp == 0);
        }
        {
          auto tmp = count_if(x, FindLessThan<int>{20}, 0);
          assert(tmp == 0);
        }
        {
          auto tmp = count_if(x, FindLessThan<int>{20}, 5);
          assert(tmp == 5);
        }
      } else {
        {
          auto tmp = count_if(x, FindLessThan<int>{10}, 0);
          assert(tmp == 10);
        }
        {
          auto tmp = count_if(x, FindLessThan<int>{20}, 0);
          assert(tmp == 20);
        }
        {
          auto tmp = count_if(x, FindLessThan<int>{20}, 5);
          assert(tmp == 25);
        }
      }
    }
  };


  struct Deref {
    template<typename I>
    ValueType<I> operator()(I x) const { return *x; }
  };

  struct Add {
    template<typename T>
    T operator()(T x, T y) const { return x+y; }
  };

  struct TestReduce {
    template<typename R>
    void operator()(R r) const {
      if (is_empty(r)) {
        auto tmp = reduce(r, Add{}, Deref{}, RangeValue<R>(0));
        assert(0 == tmp.m0);
        assert(is_empty(tmp.m1));
      } else {
        auto tmp = reduce(r, Add{}, Deref{}, RangeValue<R>(0));
        assert(39*20 == tmp.m0);
        assert(is_empty(tmp.m1));
      }
    }
  };

  struct TestReduceNonEmpty {
    template<typename R>
    void operator()(R r) const {
      if (is_empty(r)) {
        // Can't test empty ranges
      } else {
        auto tmp = reduce_nonempty(r, Add{}, Deref{});
        assert(39*20 == tmp.m0);
        assert(is_empty(tmp.m1));
      }
    }
  };

  struct TestReduceNonZeroes {
    template<typename R>
    void operator()(R r) const {
      if (is_empty(r)) {
         auto tmp = reduce_nonzeroes(r, Add{}, Deref{}, RangeValue<R>(0));
        assert(0 == tmp.m0);
        assert(is_empty(tmp.m1));
      } else {
        auto tmp = reduce_nonzeroes(r, Add{}, Deref{}, RangeValue<R>(0));
        assert(39*20 == tmp.m0);
        assert(is_empty(tmp.m1));
       }
    }
  };

  struct TestFindMismatch {
    template<typename R0, typename Iterator1, typename Count>
    void testBeginR1IsEndR0(R0 r0, Range<Iterator1, Present, Count> r1) const {
      assert(get_begin(r0) == get_end(r1));
    }

    template<typename R0, typename Iterator1, typename Count>
    void testBeginR1IsEndR0(R0 r0, Range<Iterator1, NotPresent, Count> r1) const {
      assert(get_begin(r0) == range2::advance(get_begin(r1), count));
    }

    struct FindOp {
      template<typename R0, typename R1, typename Op>
      auto operator()(R0 r0, R1 r1, Op op) -> decltype( find_mismatch(r0, r1, op) ) {
        return find_mismatch(r0, r1, op);
      }
    };

    template<typename R0, typename R1, typename Op, typename FindOp>
    void testNonEmpty(R0 r0, R1 r1, Op op, FindOp fo) const {
      auto tmp = fo(r0, r1, op);
      assert(is_empty(tmp.m0));
      testBeginR1IsEndR0(tmp.m0, r0);
      assert(is_empty(tmp.m1));
      testBeginR1IsEndR0(tmp.m1, r1);
    }

    template<typename R0, typename R1, typename Op>
    void testNonEmpties(R0 r0, R1 r1, Op op) const {
      testNonEmpty(r0, r1, op, FindOp{});
      testNonEmpty(r1, r0, op, FindOp{});
    }

    template<typename R>
    void operator()(R r) const {
      std::vector<int> x (begin, end);
      auto bounded = make_range(x.begin(), x.end(), NotPresent{});
      auto counted = make_range(x.begin(), NotPresent{}, x.size());
      auto bounded_counted = make_range(x.begin(), x.end(), x.size());
      auto unbounded = make_range(x.begin(), NotPresent{}, NotPresent{});

      auto op = make_derefop(std::equal_to<int>{});
      if (is_empty(r)) {
        {
          auto tmp = find_mismatch(r, bounded, op);
          assert(is_empty(tmp.m0));
          assert(!is_empty(tmp.m1));
          assert(get_begin(tmp.m1) == x.begin());
          assert(get_end(tmp.m1) == x.end());
        }
        {
          auto tmp = find_mismatch(r, counted, op);
          assert(is_empty(tmp.m0));
          assert(!is_empty(tmp.m1));
          assert(get_begin(tmp.m1) == x.begin());
          assert(get_count(tmp.m1) == x.size());
        }
        {
          auto tmp = find_mismatch(r, bounded_counted, op);
          assert(is_empty(tmp.m0));
          assert(!is_empty(tmp.m1));
          assert(get_begin(tmp.m1) == x.begin());
          assert(get_end(tmp.m1) == x.end());
          assert(get_count(tmp.m1) == x.size());
        }
        {
          auto tmp = find_mismatch(r, unbounded, op);
          assert(is_empty(tmp.m0));
          assert(!is_empty(tmp.m1));
          assert(get_begin(tmp.m1) == x.begin());
        }
      } else {
        testNonEmpties(r, bounded, op);
        testNonEmpties(r, counted, op);
        testNonEmpties(r, bounded_counted, op);

        {
          auto tmp = find_mismatch(r, unbounded, op);
          assert(is_empty(tmp.m0));
          testBeginR1IsEndR0(tmp.m0, r);
          // unbounded range is never empty.
          assert(!is_empty(tmp.m1));
          assert(get_begin(tmp.m1) == x.end());
        }
        auto y = x;
        y[10]=y[10]+1;
        auto tmp = find_mismatch(r, make_range(y.begin(), y.end(), y.size()), op);
        assert(!is_empty(tmp.m0));
        assert(!is_empty(tmp.m1));
        assert(y.begin() + 10 == get_begin(tmp.m1));
      }
    }
  };

  template<typename T>
  void testAdjacentMismatchInputResult(T x) {
      assert(!is_empty(x.m1));
      assert(5 == x.m0);
      assert(5 == *get_begin(x.m1));
  }

  void testFindAdjacentMismatchInput() {
    int arr[] = {1, 2, 3, 4, 5, 5, 6};
    auto op = make_derefop(std::less<int>{});
    {
      auto x = find_adjacent_mismatch_input_non_empty(make_range(&arr[0], NotPresent{}, NotPresent{}), op);
      testAdjacentMismatchInputResult(x);
    }

    {
      auto x = find_adjacent_mismatch_input_non_empty(make_range(&arr[0], &arr[0] + 6, NotPresent{}), op);
      testAdjacentMismatchInputResult(x);
    }

    {
      auto x = find_adjacent_mismatch_input_non_empty(make_range(&arr[0], NotPresent{}, 6), op);
      testAdjacentMismatchInputResult(x);
    }

    {
      auto x = find_adjacent_mismatch_input_non_empty(make_range(&arr[0], &arr[0] + 6, 6), op);
      testAdjacentMismatchInputResult(x);
    }
  }

  template<typename T>
  void testAdjacentMismatchResult(T x) {
      assert(!is_empty(x));
      assert(5 == *get_begin(x));
  }

  void testFindAdjacentMismatch() {
    int arr[] = {1, 2, 3, 4, 5, 5, 6};
    auto op = make_derefop(std::less<int>{});
    // Unbounded
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], NotPresent{}, NotPresent{}), op);
      testAdjacentMismatchResult(x);
    }

    // Bounded (by End)
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[0] + 6, NotPresent{}), op);
      testAdjacentMismatchResult(x);
    }

    // Counted
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], NotPresent{}, 6), op);
      testAdjacentMismatchResult(x);
    }

    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[0] + 6, 6), op);
      testAdjacentMismatchResult(x);
    }

    // Test empty range
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[0], NotPresent{}), op);
      assert(is_empty(x));
    }
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], NotPresent{}, 0), op);
      assert(is_empty(x));
    }
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[0], 0), op);
      assert(is_empty(x));
    }

    // Test no mismatch
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[5], NotPresent{}), op);
      assert(is_empty(x));
    }
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], NotPresent{}, 4), op);
      assert(is_empty(x));
    }
    {
      auto x = find_adjacent_mismatch(make_range(&arr[0], &arr[5], 4), op);
      assert(is_empty(x));
    }
  }

  struct TestRelationPreservingSIR {
    template<typename R>
    void operator()(R r) const {
      auto rel = make_derefop(std::less<int>{});
      assert(strictly_increasing_range(r, rel));
      auto rel2 = make_derefop(std::greater<int>{});
      assert(is_empty(r) == strictly_increasing_range(r, rel2));
    }
  };


  template<InputIterator I>
  struct input_iterator_basis {
    typedef I state_type;
    state_type position;
    typedef ValueType<I> value_type;
    typedef Reference<I> reference;
    typedef Pointer<I> pointer;
    typedef DifferenceType<I> difference_type;
    typedef std::input_iterator_tag iterator_category;

    friend constexpr ALWAYS_INLINE_HIDDEN
    reference deref(input_iterator_basis const& x) { return *x.position; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    input_iterator_basis successor(input_iterator_basis const& x) { return {range2::successor(x.position)}; }

    friend constexpr ALWAYS_INLINE_HIDDEN
    state_type state(input_iterator_basis const& x) { return x.position; }
  };


  void testRelationPreservingSIR() {
    int arr[] = {0, 1, 2, 3, 4, 4, 5};
    auto rel = make_derefop(std::less<int>{});
    assert(!strictly_increasing_range(make_range(&arr[0], &arr[0] + 7, NotPresent{}), rel));

    // Test the input iterator path.
    typedef iterator<input_iterator_basis<int*>> InputIter;
    assert(strictly_increasing_range(make_range(InputIter{{&arr[0]}}, NotPresent{}, 5), rel));
    assert(!strictly_increasing_range(make_range(InputIter{{&arr[0]}}, NotPresent{}, 7), rel));
  }

  struct TestIncreasingRange {
    template<typename R>
    void operator()(R r) const {
      auto rel = make_derefop(std::less<int>{});
      assert(increasing_range(r, rel));
      auto rel2 = make_derefop(std::greater<int>{});
      assert(is_empty(r) == increasing_range(r, rel2));
    }
  };

  void testIncreasingRange() {
    int arr[] = {0, 1, 2, 3, 4, 4, 5};
    auto rel = make_derefop(std::less<int>{});
    assert(increasing_range(make_range(&arr[0], &arr[0] + 7, NotPresent{}), rel));

    // Test the input iterator path.
    typedef iterator<input_iterator_basis<int*>> InputIter;
    assert(increasing_range(make_range(InputIter{{&arr[0]}}, NotPresent{}, 5), rel));
    assert(increasing_range(make_range(InputIter{{&arr[0]}}, NotPresent{}, 7), rel));
  }

  struct TestPartitioned {
    template<typename R>
    void operator()(R r) const {
        // Parition is false before true
      auto greaterThan = [](int x) { return x > 10; };
      auto pred = make_derefop(greaterThan);
      assert(partitioned(r, pred));
    }
  };

  void testPartitioned() {
    int arr[] = {0, 1, 2, 3, 4, 3, 3};
    auto cmp = [](int x) { return x > 3; };
    auto pred = make_derefop(cmp);

    assert(!partitioned(make_range(&arr[0], NotPresent{}, 7), pred));

    // Test the input iterator path.
    typedef iterator<input_iterator_basis<int*>> InputIter;
    assert(!partitioned(make_range(InputIter{{&arr[0]}}, NotPresent{}, 7), pred));
    assert(partitioned(make_range(InputIter{{&arr[0]}}, NotPresent{}, 5), pred));
  }

  struct TestPartitionPoint {
    template<typename R>
    void operator()(R r) const {
      auto greaterThan10 = [](int x) { return x > 10; };
      auto pred = make_derefop(greaterThan10);

      auto greaterEqualCount = [](int x) { return x >= count; };
      auto pred2 = make_derefop(greaterEqualCount);
      if (is_empty(r)) {
        auto tmp = partition_point(r, pred);
        assert(is_empty(tmp.m0));
        assert(is_empty(tmp.m1));
      } else {
        {
          auto tmp = partition_point(r, pred);
          assert(!is_empty(tmp.m0));
          assert(!is_empty(tmp.m1));
          assert(11 == *get_begin(tmp.m1));
        }
        {
          auto tmp = partition_point(r, pred2);
          assert(!is_empty(tmp.m0));
          assert(is_empty(tmp.m1));
        }
      }    
    }
  };

  // Also tests lower and upper bound
  void testEquivalentRange() {
    typedef int Value;
    typedef std::vector<Value> Vec;
    constexpr Value equal_value = 101;
    Vec prefix(equal_value-1);
    std::iota(prefix.begin(), prefix.end(), 0);
    Vec postfix(equal_value-1);
    std::iota(postfix.begin(), postfix.end(), equal_value + 1);
    Vec eq_range(100, equal_value);
    for (int i = 0; i <= prefix.size(); ++i ) {
      for (int j = 0; j <= eq_range.size(); ++j) {
        for (int k = 0; k <= postfix.size(); ++k) {
          Vec toSearch;
          toSearch.reserve(i+j+k);
          toSearch.insert(toSearch.end(), &prefix[0], &prefix[0] + i);
          toSearch.insert(toSearch.end(), &eq_range[0], &eq_range[0] + j);
          toSearch.insert(toSearch.end(), &postfix[0], &postfix[0] + k);

          {
            auto range0 = make_range(std::begin(toSearch), std::end(toSearch), NotPresent{});
            auto tmp0 = equivalent_range(range0, std::less<Value>{}, equal_value);

            assert(std::begin(toSearch) == get_begin(tmp0.m0));
            assert(std::begin(toSearch) + i == get_end(tmp0.m0));
            assert(i == get_count(tmp0.m0));

            assert(std::begin(toSearch) + i == get_begin(tmp0.m1));
            assert(std::begin(toSearch) + i + j == get_end(tmp0.m1));
            assert(j == get_count(tmp0.m1));

            assert(std::begin(toSearch) + i + j == get_begin(tmp0.m2));
            assert(k == get_count(tmp0.m2));
            assert(get_end(range0) == get_end(tmp0.m2));
          }
          {
            auto range1 = make_range(std::begin(toSearch), std::end(toSearch), toSearch.size());
            auto tmp1 = equivalent_range(range1, std::less<Value>{}, equal_value);

            assert(std::begin(toSearch) == get_begin(tmp1.m0));
            assert(std::begin(toSearch) + i == get_end(tmp1.m0));
            assert(i == get_count(tmp1.m0));

            assert(std::begin(toSearch) + i == get_begin(tmp1.m1));
            assert(std::begin(toSearch) + i + j == get_end(tmp1.m1));
            assert(j == get_count(tmp1.m1));

            assert(std::begin(toSearch) + i + j == get_begin(tmp1.m2));
            assert(k == get_count(tmp1.m2));
            assert(get_end(range1) == get_end(tmp1.m2));
          }
        }
      }
    }
  }

  struct TestLexicographicalEqual {
    template<typename Iterator>
    void operator()(Range<Iterator, NotPresent, NotPresent> r) const {
      assert(!lexicographical_equal(r, successor(r)));
      assert(!lexicographical_equal(successor(r), r));
    }

    template<typename R>
    void operator()(R r) const {
      assert(lexicographical_equal(r, r));
      if (is_empty(r)) {
        assert(!lexicographical_equal(r, make_range(begin, end, NotPresent{})));
        assert(!lexicographical_equal(make_range(begin, end, NotPresent{}), r));
      } else {
        assert(lexicographical_equal(r, make_range(begin, end, NotPresent{})));
        assert(lexicographical_equal(make_range(begin, end, NotPresent{}), r));

        assert(!lexicographical_equal(r, successor(r)));
        assert(!lexicographical_equal(successor(r), r));
      }
    }
  };

  template<typename Range0, typename Range1>
  void testLexicographicalLessImpl(Range0 r0, Range1 r1) {
    assert(lexicographical_less(r0, r1));
    assert(!lexicographical_less(r1, r0));
  }

  void testLexicographicalLess() {
    int arr[] = {0, 1, 2, 3, 4};
    auto r = make_range(&arr[0], NotPresent{}, 5);
    auto n = successor(r);
    testLexicographicalLessImpl(r, n);
    testLexicographicalLessImpl(make_range(get_begin(r), NotPresent{}, get_count(r) - 1), r);
  }

  struct TestLexicographicalLess {
    template<typename R>
    void operator()(R r) const {
      if (is_empty(r)) {
        testLexicographicalLessImpl(r, make_range(begin, NotPresent{}, NotPresent{}));
      } else {
        assert(!lexicographical_less(r, r));

        // Different lengths but values within successor(r) are greater by one
        testLexicographicalLessImpl(r, successor(r));

        // Different lengths
        testLexicographicalLessImpl(make_range(get_begin(r), NotPresent{}, count-1), r);
      }
    }
  };

  struct TYPE_HIDDEN_VISIBILITY MoveOnlyType
  {
    static constexpr int NOT_SET_TAG = -1;

    int tag;
    ALWAYS_INLINE_HIDDEN MoveOnlyType() : tag(NOT_SET_TAG) {}

    MoveOnlyType(MoveOnlyType const&) = delete;
    MoveOnlyType& operator=(MoveOnlyType const&) = delete;

    ALWAYS_INLINE_HIDDEN MoveOnlyType(MoveOnlyType&& x) : tag(x.tag) { x.tag = NOT_SET_TAG; };
    ALWAYS_INLINE_HIDDEN MoveOnlyType& operator=(MoveOnlyType&& x) { tag = x.tag; x.tag = NOT_SET_TAG; return *this; }
  };

  struct TYPE_HIDDEN_VISIBILITY SwapOnlyType
  {
    int tag;
    SwapOnlyType() : tag(0) {}
    SwapOnlyType(SwapOnlyType const&) = delete;
    SwapOnlyType(SwapOnlyType&&) = delete;
    SwapOnlyType& operator=(SwapOnlyType const&) = delete;
    SwapOnlyType& operator=(SwapOnlyType&&) = delete;

    friend
    void swap(SwapOnlyType& x, SwapOnlyType &y) {
      using std::swap;
      swap(x.tag, y.tag);
    }
  };

  void testSteps() {
    const int arri[] = {0, 1};
    int arro[] = {2, 3};
    auto r0 = make_range(&arri[0], NotPresent{}, 2);
    auto r1 = make_range(&arro[0] + 2, NotPresent{}, 2);

    auto i0 = r0;
    auto o0 = r1;
    copy_step{}(i0, o0);
    assert(i0 == successor(r0));
    assert(o0 == successor(r1));
    assert(*get_begin(r0) == *get_begin(r1));

    MoveOnlyType arr2[2];
    arr2[0].tag = 0;
    arr2[1].tag = 1;
    auto r2 = make_range(&arr2[0], NotPresent{}, 1);
    auto r3 = make_range(&arr2[0] + 1, NotPresent{}, 1);
    auto i1 = r2;
    auto o1 = r3;
    move_step{}(i1, o1);
    assert(successor(r2) == i1);
    assert(successor(r3) == o1);
    assert(get_begin(r2)->tag == MoveOnlyType::NOT_SET_TAG);
    assert(get_begin(r3)->tag == 0);

    SwapOnlyType arr3[2];
    arr3[0].tag = 1;
    arr3[1].tag = 2;
    auto r4 = make_range(&arr3[0], NotPresent{}, 1);
    auto r5 = make_range(&arr3[0] + 1, NotPresent{}, 1);
    auto i2 = r4;
    auto o2 = r5;
    swap_step{}(i2, o2);
    assert(successor(r4) == i2);
    assert(successor(r5) == o2);
    assert(get_begin(r4)->tag == 2);
    assert(get_begin(r5)->tag == 1);
  }

  void testVisit2Ranges() {
    const int zeroes[count] = {};
    auto zeroesRange = make_range(&zeroes[0], NotPresent{}, count);
    int arr[count] = {};
    auto inputRange = make_range(begin, end, count);
    auto outputRange = make_range(&arr[0], &arr[0] + count, count);
    auto tmp = visit_2_ranges(inputRange, outputRange, copy_step{});
    assert(is_empty(tmp.m0));
    assert(end == get_begin(tmp.m0));
    assert(end == get_end(tmp.m0));
    assert(0 == get_count(tmp.m0));

    assert(is_empty(tmp.m1));
    assert(&arr[0]+count == get_begin(tmp.m1));
    assert(&arr[0]+count == get_end(tmp.m1));
    assert(0 == get_count(tmp.m1));
    assert(lexicographical_equal(inputRange, outputRange));

    visit_2_ranges(zeroesRange, outputRange, copy_step{});

    assert(!lexicographical_equal(inputRange, outputRange));
    auto tmp2 = visit_2_ranges(inputRange, make_range(&arr[0], NotPresent{}, NotPresent{}), copy_step{});
    assert(is_empty(tmp2.m0));
    assert(end == get_begin(tmp2.m0));
    assert(end == get_end(tmp2.m0));
    assert(0 == get_count(tmp2.m0));

    assert(!is_empty(tmp2.m1));
    assert(&arr[0] + count == get_begin(tmp2.m1));
    NotPresent np = get_end(tmp2.m1);
    np = get_count(tmp2.m1);
    assert(lexicographical_equal(inputRange, outputRange));

    visit_2_ranges(zeroesRange, outputRange, copy_step{});

    assert(!lexicographical_equal(inputRange, outputRange));
    auto tmp3 = visit_2_ranges(make_range(begin, NotPresent{}, NotPresent{}), outputRange, copy_step{});
    assert(!is_empty(tmp3.m0));
    assert(end == get_begin(tmp3.m0));
    np = get_end(tmp3.m0);
    np = get_count(tmp3.m0);

    assert(is_empty(tmp3.m1));
    assert(&arr[0] + count == get_begin(tmp3.m1));
    assert(get_end(tmp3.m1) == get_begin(tmp3.m1));
    assert(0 == get_count(tmp3.m1));
    assert(lexicographical_equal(inputRange, outputRange));
  }

  void testVisit3Ranges() {
    constexpr int ARR_LEN = 10;
    int arr[ARR_LEN] = {};
    int arr2[ARR_LEN] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int arr3[ARR_LEN] = {};
    auto unbounded0 = make_range(&arr[0], NotPresent{}, NotPresent{});
    auto unbounded1 = make_range(&arr2[0], NotPresent{}, NotPresent{});
    auto bounded0 = make_range(&arr3[0], NotPresent{}, ARR_LEN);
    auto pred = make_derefop([](int, int) { return true; });
    visit_3_ranges(unbounded0, unbounded1, bounded0, make_merge_if(pred, copy_step{}));
    assert(lexicographical_equal(make_range(&arr2[0], NotPresent{}, ARR_LEN), make_range(&arr3[0], NotPresent{}, ARR_LEN)));

    visit_3_ranges(make_range(&arr[0], NotPresent{}, ARR_LEN), make_range(&arr2[0], NotPresent{}, ARR_LEN), bounded0, make_merge_if(pred, copy_step{}));
    assert(lexicographical_equal(make_range(&arr2[0], NotPresent{}, ARR_LEN), make_range(&arr3[0], NotPresent{}, ARR_LEN)));
  }
} // unnamed namespace
} // namespace range2

using namespace range2;

int main() {
  testPair();
  testTriple();
  testPresentNotPresent();

  testGetBegin();
  testGetEnd();
  testGetCount();

  testAddEnd();
  testAddCount();

  // Test depends on ability to get & add both end & count.
  testRangeRelationals();

  testRemoveEnd();
  testRemoveCount();

  testAddConstantTimeEnd();
  testAddConstantTimeCount();

  testAddLinearTimeEnd();
  testAddLinearTimeCount();

  testIsEmpty();
  testSuccessor();

  testSplitAt();
  testJoin();

  testIterator();
  testReversedIterator();
  testSkipIterator();

  testReverse();
  testSkip();
  testReverseSkip();

  forEachRangeRun(TestForEachOp{});
  forEachRangeRun(TestFindIfOp{});
  forEachRangeRun(TestCountIfOp{});
  forEachRangeRun(TestReduce{});
  forEachRangeRun(TestReduceNonEmpty{});
  forEachRangeRun(TestReduceNonZeroes{});
  forEachRangeRun(TestFindMismatch{});
  testFindAdjacentMismatchInput();
  testFindAdjacentMismatch();
  forEachRangeRun(TestRelationPreservingSIR{});
  testRelationPreservingSIR();
  forEachRangeRun(TestIncreasingRange{});
  testIncreasingRange();
  forEachRangeRun(TestPartitioned{});
  testPartitioned();
  forEachRangeRun(TestPartitionPoint{});
  testEquivalentRange();
  forEachRangeRun(TestLexicographicalEqual{});
  testLexicographicalLess();
  forEachRangeRun(TestLexicographicalLess{});

  testSteps();
  testVisit2Ranges();
  testVisit3Ranges();

  testPerformance();
}
