#include "range2.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <forward_list>
#include <chrono>

namespace range2 {
namespace {

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

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  typedef int const* PtrType;

  constexpr int arr[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  constexpr PtrType begin = &arr[0];
  constexpr PtrType end = &arr[20];
  constexpr int count = 20;
  const std::forward_list<int> slist(begin, end);

  // Test Range construction
  constexpr Range<PtrType, NotPresent, NotPresent> r00 = {begin};
  constexpr Range<PtrType, NotPresent, Present> r01 = {begin, count};
  constexpr Range<PtrType, Present, NotPresent> r10 = {begin, end};
  constexpr Range<PtrType, Present, Present> r11 = {begin, end, count};

  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<PtrType, Present, Present>>, std::random_access_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::istream_iterator<char>, Present, Present>>, std::input_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::forward_list<char>::iterator, Present, Present>>, std::forward_iterator_tag>::value));

  void testEquality() {
    TEST_ASSERT(r00 == r00);
    TEST_ASSERT(r01 == r01);
    TEST_ASSERT(r10 == r10);
    TEST_ASSERT(r11 == r11);
  }

  void testInequality() {
    TEST_ASSERT(!(r00 != r00));
    TEST_ASSERT(!(r01 != r01));
    TEST_ASSERT(!(r10 != r10));
    TEST_ASSERT(!(r11 != r11));
  }

  void testGetBegin() {
    TEST_ASSERT(begin == getBegin(r00));
    TEST_ASSERT(begin == getBegin(r01));
    TEST_ASSERT(begin == getBegin(r10));
    TEST_ASSERT(begin == getBegin(r11));

    // Test non-const reference path
    Range<PtrType, NotPresent, NotPresent> tmp = {begin};
    PtrType& begin2 = getBegin(tmp);
    assert(begin2 == begin);
  }

  void testGetEnd() {
    TEST_ASSERT(end == getEnd(r10));
    TEST_ASSERT(end == getEnd(r11));

    // Test non-const reference path
    Range<PtrType, Present, NotPresent> tmp = {begin, end};
    PtrType& end2 = getEnd(tmp);
    assert(end2 == end);

    NotPresent tmp2 = getEnd(r00);
    (void)tmp2;

    NotPresent tmp3 = getEnd(r01);
    (void)tmp3;
  }

  void testGetCount() {
    TEST_ASSERT(count == getCount(r01));
    TEST_ASSERT(count == getCount(r11));

    // Test non-const reference path
    Range<PtrType, Present, Present> tmp = {begin, end, count};
    CountType<PtrType>& count2 = getCount(tmp);
    assert(count2 == count);

    NotPresent tmp2 = getCount(r00);
    (void)tmp2;

    NotPresent tmp3 = getCount(r10);
    (void)tmp3;
  }

  void testAddEnd() {
    TEST_ASSERT(end == getEnd(addEnd(r00, end)));
    TEST_ASSERT(end == getEnd(addEnd(r01, end)));
    TEST_ASSERT(end == getEnd(addEnd(r10, end)));
    TEST_ASSERT(end == getEnd(addEnd(r11, end)));
  }

  void testAddCount() {
    constexpr int x = 123;

    TEST_ASSERT(x == getCount(addCount(r00, x)));
    TEST_ASSERT(x == getCount(addCount(r01, x)));
    TEST_ASSERT(x == getCount(addCount(r10, x)));
    TEST_ASSERT(x == getCount(addCount(r11, x)));
  }

  void testRemoveEnd() {
    {
      auto tmp = removeEnd(r10);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removeEnd(r11);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }
  }

  void testRemoveCount() {
    {
      auto tmp = removeCount(r01);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removeCount(r11);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }
  }

  template<typename T>
  void testAddConstantTimeEndImpl(T x) {
      auto tmp = addConstantTimeEnd(x);
      assert(getBegin(x) == getBegin(tmp));
      assert(end == getEnd(tmp));
      assert(getCount(x) == getCount(tmp));
  }

  template<typename T>
  void testAddConstantTimeEndNotPossibleImpl(T x) {
      auto tmp = addConstantTimeEnd(x);
      assert(getBegin(x) == getBegin(tmp));
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
      assert(getCount(x) == getCount(tmp));
  }

  void testAddConstantTimeEnd() {
    testAddConstantTimeEndImpl(r01);
    testAddConstantTimeEndImpl(r10);
    testAddConstantTimeEndImpl(r11);

    testAddConstantTimeEndNotPossibleImpl(r00);
    testAddConstantTimeEndNotPossibleImpl(makeRange(slist.begin(), NotPresent{}, 20));
  }

  template<typename T>
  void testAddConstantTimeCountImpl(T x) {
    auto tmp = addConstantTimeCount(x);
    assert(getBegin(x) == getBegin(tmp));
    assert(getEnd(x) == getEnd(tmp));
    assert(count == getCount(tmp));
  }

  template<typename T>
  void testAddConstantTimeCountNotPossibleImpl(T x) {
      auto tmp = addConstantTimeCount(x);
      assert(getBegin(x) == getBegin(tmp));
      assert(getEnd(x) == getEnd(tmp));
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
  }

  void testAddConstantTimeCount() {
    testAddConstantTimeCountImpl(r10);
    testAddConstantTimeCountImpl(r01);
    testAddConstantTimeCountImpl(r11);

    testAddConstantTimeCountNotPossibleImpl(r00);
    testAddConstantTimeCountNotPossibleImpl(makeRange(slist.begin(), slist.end(), NotPresent{}));
  }

  template<typename T>
  void testAddLinearTimeEndImpl(T x) {
    auto tmp = addLinearTimeEnd(x);
    assert(getBegin(x) == getBegin(tmp));
    assert(end == getEnd(tmp));
    assert(count == getCount(tmp));
  }

  template<typename T>
  void testAddLinearTimeEndNoCountCheckImpl(T x) {
    auto tmp = addLinearTimeEnd(x);
    assert(getBegin(x) == getBegin(tmp));
    assert(end == getEnd(tmp));
  }

  void testAddLinearTimeEnd() {
    testAddLinearTimeEndImpl(r01);
    testAddLinearTimeEndImpl(r11);
    testAddLinearTimeEndImpl(r11);

    {
      auto tmp = addLinearTimeEnd(makeRange(slist.begin(), NotPresent{}, 20));
      assert(slist.end() == getEnd(tmp));
      assert(20 == getCount(tmp));
    }
  }

  template<typename T>
  void testAddLinearTimeCountImpl(T x) {
    auto tmp = addLinearTimeCount(x);
    assert(count == getCount(tmp));
    assert(end == getEnd(tmp));
  }

  template<typename T>
  void testAddLinearTimeCountNoEndCheckImpl(T x) {
    auto tmp = addLinearTimeCount(x);
    assert(count == getCount(tmp));
  }

  void testAddLinearTimeCount() {
    // Can't call for r00 as this don't have a termiantion condition.
    testAddLinearTimeCountImpl(r10);
    testAddLinearTimeCountImpl(r11);

    {
      auto tmp = addLinearTimeCount(makeRange(slist.begin(), slist.end(), NotPresent{}));
      assert(20 == getCount(tmp));
      assert(slist.end() == getEnd(tmp));
    }
  }

  void testIsEmpty() {
    assert(!isEmpty(r00));
    assert(!isEmpty(r01));
    assert(!isEmpty(r10));
    assert(!isEmpty(r11));
    
    {
      auto tmp = makeRange(begin, begin, NotPresent{});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 0);
      assert(isEmpty(tmp));
    }

    {
      // Demonstrates that count is used in preference to begin/end pair
      auto tmp = makeRange(begin, end, 0);
      assert(isEmpty(tmp));
    }
  }

  template<typename Range>
  void testBoundedRange(Range x) {
    int i = 0;
    while(!isEmpty(x)) {
      assert(i == *getBegin(x));
      ++i;
      x = next(x);
    }
    assert(20 == i);
  }

  void testNext() {
    testBoundedRange(makeRange(begin, end, NotPresent{}));
    testBoundedRange(makeRange(begin, NotPresent{}, 20));
    testBoundedRange(makeRange(begin, end, 20));
  }

  template<typename R0, typename R1, typename R2>
  void checkBoundedness(R0 const&, pair<R1, R2> const&) {
    static_assert(IsABoundedRange<R1>::value, "Must be a bounded range");
    static_assert(IsABoundedRange<R0>::value == IsABoundedRange<R2>::value, "Boundedness must be equal");
  }

  void testSplitAt() {
    // For r00, r01, r10 & r11 need to check the case of providing
    // a middle, an end, and both a middle and an end. Note that providing just an end forces splitAt
    // to calculate the middle too.
    {
      auto tmp = splitAt(r00, begin + 9, NotPresent{});
      checkBoundedness(r00, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      NotPresent np0 = getCount(tmp.m0);
      (void)np0;

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np1 = getEnd(tmp.m1);
      (void)np1;
      NotPresent np2 = getCount(tmp.m1);
      (void)np2;
    }

    {
      auto tmp = splitAt(r00, NotPresent{}, 9);
      checkBoundedness(r00, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np0 = getEnd(tmp.m1);
      (void)np0;
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r00, begin + 9, 9);
      checkBoundedness(r00, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np0 = getEnd(tmp.m1);
      (void)np0;
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r01, begin + 9, NotPresent{});
      checkBoundedness(r01, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      // count has to be calculated anyhow
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np0 = getEnd(tmp.m1);
      (void)np0;
      assert(11 == getCount(tmp.m1));
    }

    {
      auto tmp = splitAt(r01, NotPresent{}, 9);
      checkBoundedness(r01, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np0 = getEnd(tmp.m1);
      (void)np0;
      assert(11 == getCount(tmp.m1));
    }

    {
      auto tmp = splitAt(r01, begin + 9, 9);
      checkBoundedness(r01, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      NotPresent np0 = getEnd(tmp.m1);
      (void)np0;
      assert(11 == getCount(tmp.m1));
    }

    {
      auto tmp = splitAt(r10, begin + 9, NotPresent{});
      checkBoundedness(r10, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + 9 == getEnd(tmp.m0));
      NotPresent np0 = getCount(tmp.m0);
      (void)np0;

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r10, NotPresent{}, 9);
      checkBoundedness(r10, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      // Middle has to be calculated anyway
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r10, begin + 9, 9);
      checkBoundedness(r10, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r11, begin + 9, NotPresent{});
      checkBoundedness(r11, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      NotPresent np0 = getCount(tmp.m0);
      (void)np0;

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      NotPresent np1 = getCount(tmp.m1);
      (void)np1;
    }

    {
      auto tmp = splitAt(r11, NotPresent{}, 9);
      checkBoundedness(r11, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      // Needs to calculate middle anyhow
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      assert(11 == getCount(tmp.m1));
    }

    {
      auto tmp = splitAt(r11, begin + 9, 9);
      checkBoundedness(r11, tmp);

      assert(!isEmpty(tmp.m0));
      assert(begin == getBegin(tmp.m0));
      assert(begin + 9 == getEnd(tmp.m0));
      assert(9 == getCount(tmp.m0));

      assert(!isEmpty(tmp.m1));
      assert(begin + 9 == getBegin(tmp.m1));
      assert(end == getEnd(tmp.m1));
      assert(11 == getCount(tmp.m1));
    }
  }

  template<typename R0, typename R1, typename Result>
  void testJoinBoundedness(R0 const&, R1 const&, Result const&) {
    static_assert(IsABoundedRange<R1>::value == IsABoundedRange<Result>::value, "Error");
  }

  template<typename R>
  void testJoinImpl(R const& rng) {
    {
      auto r = makeRange(begin + 9, NotPresent{}, NotPresent{});
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == getBegin(tmp));
      NotPresent np0 = getEnd(tmp);
      np0 = getCount(tmp);
    }

    {
      auto r = makeRange(begin + 9, end, NotPresent{});
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == getBegin(tmp));
      assert(end == getEnd(tmp));
      NotPresent np0 = getCount(tmp);
      (void)np0;
    }

    {
      auto r = makeRange(begin + 9, NotPresent{}, 11);
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == getBegin(tmp));
      assert(end == getEnd(tmp));
    }

    {
      auto r = makeRange(begin + 9, end, 11);
      auto tmp = join(rng, r);
      testJoinBoundedness(rng, r, tmp);
      assert(begin == getBegin(tmp));
      assert(end == getEnd(tmp));
    }
  }

  void testJoin() {
    testJoinImpl(r00);
    testJoinImpl(r01);
    testJoinImpl(r10);
    testJoinImpl(r11);
  }


  void testIterator() {
    iterator<iterator_basis<int const*>> tmp = {{begin}};
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
    iterator<iterator_basis<int*>> tmp6 = {{&arr[0]}};
    sink(tmp6 + 2, 55);
    assert(55 == tmp6[2]);

    // Test operator ->
  }

  typedef unsigned long long SumType;

  template<typename T>
  SumType sumOver(T x) {
    SumType tmp = 0;
    while (!isEmpty(x)) {
      tmp += *getBegin(x);
      x = next(x);
    }
    return tmp;
  }

  template<typename T>
  SumType sumOver2(T x) {
    SumType tmp0 = 0;
    SumType tmp1 = 0;
    SumType tmp2 = 0;
    SumType tmp3 = 0;
    auto count = getCount(x);
    auto cBy4 = count>>2;
    count -= cBy4 << 2;
    auto begin = getBegin(x);
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

  constexpr int loopTimes = 100;

  template<typename T>
  void performanceTest2(T x) {
    SumType sum = 0U;
    timer t;
    t.start();
    for (int i=0; i < loopTimes; ++i) {
      sum += sumOver(x);
    }
    auto time = t.stop();
    std::cout << sum << ' ' << time << std::endl;
  }

  template<typename T>
  void performanceTest3(T x) {
    SumType sum = 0U;
    timer t;
    t.start();
    for (int i=0; i < loopTimes; ++i) {
      sum += sumOver2(x);
    }
    auto time = t.stop();
    std::cout << sum << ' ' << time << std::endl;
  }

  template<typename I>
  iterator<iterator_basis<I>> make_iterator(I x) {
    return {{x}};
  }

  void testPerformance() {
    std::vector<SumType> v(1000000);
    std::iota(v.begin(), v.end(), 5);
    auto r0 = makeRange(v.begin(), v.end(), NotPresent{});
    auto r1 = makeRange(v.begin(), NotPresent{}, v.size());
    auto r2 = makeRange(v.begin(), v.end(), v.size());

    auto r3 = makeRange(make_iterator(v.begin()), make_iterator(v.end()), NotPresent{});
    auto r4 = makeRange(make_iterator(v.begin()), NotPresent{}, v.size());
    auto r5 = makeRange(make_iterator(v.begin()), make_iterator(v.end()), v.size());

    sumOver(r0);
    performanceTest2(r0);
    performanceTest2(r1);
    performanceTest2(r2);
    performanceTest2(r3);
    performanceTest2(r4);
    performanceTest2(r5);

    performanceTest3(r1);
    performanceTest3(r2);
    performanceTest3(r4);
    performanceTest3(r5);
    timer t;
    t.start();
    SumType s =0U;
    for(int i=0; i < loopTimes; ++i) {
      s += std::accumulate(v.begin(), v.end(), 0);
    }
    auto tmp = t.stop();
    std::cout << s << ' ' << tmp << std::endl;
  }
} // unnamed namespace
} // namespace range2

using namespace range2;

int main() {
  testEquality();
  testInequality();

  testGetBegin();
  testGetEnd();
  testGetCount();

  testAddEnd();
  testAddCount();

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

  reverse_iterator<int const*> a = {{begin}};
  reverse_iterator<int const*> b = {{end}};
  auto tmp = makeRange(b, a, NotPresent{});
  int expected = 19;
  while (!isEmpty(tmp)) {
    assert(expected == *getBegin(tmp));
    --expected;
    tmp = next(tmp);
  }
  assert(20 == std::distance(b, a));
}
