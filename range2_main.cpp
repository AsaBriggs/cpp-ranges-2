#include "range2.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <forward_list>
#include <chrono>

namespace range2 {
namespace {

// timer taken from
// https://github.com/psoberoi/stepanov-conversations-course/blob/master/tape/timer.h
//
class timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double stop() {
        auto stop_time = std::chrono::high_resolution_clock::now();
        return double(std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count());
    }
};

// end of timer copy.

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

  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<PtrType, Present, Present>>, std::random_access_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::istream_iterator<char>, Present, Present>>, std::input_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::forward_list<char>::iterator, Present, Present>>, std::forward_iterator_tag>::value));

  template<typename T>
  void testRelationalOperators(T lesser, T greater) {
    assert(lesser == lesser);
    assert(!(lesser != lesser));

    assert(greater == greater);
    assert(!(greater != greater));

    assert(lesser != greater);
    assert(!(lesser == greater));

    assert(!(lesser < lesser));
    assert(lesser <= lesser);
    assert(lesser >= lesser);
    assert(!(lesser > lesser));

    assert(!(greater < greater));
    assert(greater <= greater);
    assert(greater >= greater);
    assert(!(greater > greater));

    assert(lesser < greater);
    assert(lesser <= greater);
    assert(!(lesser > greater));
    assert(!(lesser >= greater));

    assert(!(greater < lesser));
    assert(!(greater <= lesser));
    assert(greater > lesser);
    assert(greater >= lesser);
  }

  void testPair() {
    constexpr auto x = make_pair(1, 2);
    constexpr auto y = make_pair(2, 3);
    testRelationalOperators(x, y);
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
    CountType<PtrType>& count2 = get_count(tmp);
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
    testRelationalOperators(r00, next(r00));
    testRelationalOperators(r01, next(r01));
    testRelationalOperators(r10, next(r10));
    testRelationalOperators(r11, next(r11));

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
    while(!is_empty(x)) {
      assert(i == *get_begin(x));
      ++i;
      x = next(x);
    }
    assert(count == i);
  }

  void testNext() {
    testBoundedRange(make_range(begin, end, NotPresent{}));
    testBoundedRange(make_range(begin, NotPresent{}, count));
    testBoundedRange(make_range(begin, end, count));
  }

  template<typename R0, typename R1, typename R2>
  void checkBoundedness(R0 const&, pair<R1, R2> const&) {
    static_assert(IsABoundedRange<R1>::value, "Must be a bounded range");
    static_assert(IsABoundedRange<R0>::value == IsABoundedRange<R2>::value, "Boundedness must be equal");
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
    static_assert(IsABoundedRange<R1>::value == IsABoundedRange<Result>::value, "Error");
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
    while(Start != End) {
      assert(expected == *Start);
      --expected;
      ++Start;
    }
    assert(-1 == expected);

    // check predecessor
    expected = 0;
    Start = make_reverse_iterator(end);
    while(Start != End) {
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
    while(Start != End) {
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
      range = next(range);
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
    while(!is_empty(y)) {
      assert(expected == *get_begin(y));
      expected += N;
      y = next(y);
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
      x = next(x);
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
      x = next(x);
    }
    return tmp;
  }

  template<typename T>
  SumType unrolledSumOver1(T x) {
    auto count = get_count(x);
    auto begin = get_begin(x);
    SumType tmp = 0;
    while(count) {
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
    while (cBy2) {
      tmp0 += *begin;
      tmp1 += *(begin+1);
      begin += 2;
      --cBy2;
    }
    if(count) {
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
    while (cBy4) {
      tmp0 += *begin;
      tmp1 += *(begin+1);
      tmp2 += *(begin+2);
      tmp3 += *(begin+3);
      begin += 4;
      --cBy4;
    }
    SumType tmp = tmp0 + tmp1 + tmp2 + tmp3;
    while(count) {
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
    while (cBy4) {
      auto r1 = next(range);
      auto r2 = next(r1);
      auto r3 = next(r2);
      tmp0 += *get_begin(range);
      tmp1 += *get_begin(r1);
      tmp2 += *get_begin(r2);
      tmp3 += *get_begin(r3);
      --cBy4;
      range = next(r3);
    }
    SumType tmp4 = 0;
    while(count) {
      tmp4 += *get_begin(range);
      range = next(range);
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
    while (cBy8) {
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
    while(count) {
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
   }
} // unnamed namespace
} // namespace range2

using namespace range2;

int main() {
  testPair();
  testPresentNotPresent();

  testGetBegin();
  testGetEnd();
  testGetCount();

  testAddEnd();
  testAddCount();

  // Test depends on ability to get & add both end & count.z
  testRangeRelationals();

  testRemoveEnd();
  testRemoveCount();

  testAddConstantTimeEnd();
  testAddConstantTimeCount();

  testAddLinearTimeEnd();
  testAddLinearTimeCount();

  testIsEmpty();
  testNext();

  testSplitAt();
  testJoin();

  testPerformance();

  testIterator();
  testReversedIterator();
  testSkipIterator();

  testReverse();
  testSkip();
  testReverseSkip();
}
