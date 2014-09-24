#include "range2.h"

#include <cassert>
#include <iostream>
#include <forward_list>

namespace range2 {
namespace {

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  typedef int const* PtrType;

  struct StatelessFalsePredicate
  {
    constexpr bool operator()(PtrType) const { return false; }

    friend
    constexpr bool operator==(StatelessFalsePredicate const& x, StatelessFalsePredicate const& y) { return true; }
  };

  struct StatelessTruePredicate
  {
    constexpr bool operator()(PtrType) const { return true; }

    friend
    constexpr bool operator==(StatelessTruePredicate const& x, StatelessTruePredicate const& y) { return true; }
  };

  struct StatefulPredicate
  {
    bool state;
    constexpr bool operator()(PtrType) const { return state; }

    friend
    constexpr bool operator==(StatefulPredicate const& x, StatefulPredicate const& y) { return x.state == y.state; }
  };

  constexpr int arr[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  constexpr PtrType begin = &arr[0];
  constexpr PtrType end = &arr[20];
  constexpr int count = 20;
  constexpr StatefulPredicate statefulState = {false};
  const std::forward_list<int> slist(begin, end);

  // Test Range construction
  constexpr Range<PtrType, NotPresent, NotPresent, NotPresent> r000 = {begin};

  constexpr Range<PtrType, NotPresent, NotPresent, StatelessFalsePredicate> r001 = {begin};
  constexpr Range<PtrType, NotPresent, NotPresent, StatefulPredicate> r002 = {begin, statefulState};

  constexpr Range<PtrType, NotPresent, Present, NotPresent> r010 = {begin, count};
  constexpr Range<PtrType, NotPresent, Present, StatelessFalsePredicate> r011 = {begin, count};
  constexpr Range<PtrType, NotPresent, Present, StatefulPredicate> r012 = {begin, count, statefulState};

  constexpr Range<PtrType, Present, NotPresent, NotPresent> r100 = {begin, end};

  constexpr Range<PtrType, Present, NotPresent, StatelessFalsePredicate> r101 = {begin, end};
  constexpr Range<PtrType, Present, NotPresent, StatefulPredicate> r102 = {begin, end, statefulState};

  constexpr Range<PtrType, Present, Present, NotPresent> r110 = {begin, end, count};
  constexpr Range<PtrType, Present, Present, StatelessFalsePredicate> r111 = {begin, end, count};
  constexpr Range<PtrType, Present, Present, StatefulPredicate> r112 = {begin, end, count, statefulState};

  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<PtrType, Present, Present, NotPresent>>, std::random_access_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<PtrType, Present, Present, StatefulPredicate>>, std::forward_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::istream_iterator<char>, Present, Present, NotPresent>>, std::input_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::istream_iterator<char>, Present, Present, StatefulPredicate>>, std::input_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::forward_list<char>::iterator, Present, Present, NotPresent>>, std::forward_iterator_tag>::value));
  TEST_ASSERT((std::is_same<RangeEffectiveIteratorCategory<Range<std::forward_list<char>::iterator, Present, Present, StatefulPredicate>>, std::forward_iterator_tag>::value));

  void testEquality() {
    TEST_ASSERT(r000 == r000);
    TEST_ASSERT(r001 == r001);
    TEST_ASSERT(r002 == r002);

    TEST_ASSERT(r010 == r010);
    TEST_ASSERT(r011 == r011);
    TEST_ASSERT(r012 == r012);

    TEST_ASSERT(r100 == r100);
    TEST_ASSERT(r101 == r101);
    TEST_ASSERT(r102 == r102);

    TEST_ASSERT(r110 == r110);
    TEST_ASSERT(r111 == r111);
    TEST_ASSERT(r112 == r112);
  }

  void testInequality() {
    TEST_ASSERT(!(r000 != r000));
    TEST_ASSERT(!(r001 != r001));
    TEST_ASSERT(!(r002 != r002));

    TEST_ASSERT(!(r010 != r010));
    TEST_ASSERT(!(r011 != r011));
    TEST_ASSERT(!(r012 != r012));

    TEST_ASSERT(!(r100 != r100));
    TEST_ASSERT(!(r101 != r101));
    TEST_ASSERT(!(r102 != r102));

    TEST_ASSERT(!(r110 != r110));
    TEST_ASSERT(!(r111 != r111));
    TEST_ASSERT(!(r112 != r112));
  }

  void testGetBegin() {
    TEST_ASSERT(begin == getBegin(r000));
    TEST_ASSERT(begin == getBegin(r001));
    TEST_ASSERT(begin == getBegin(r002));
    TEST_ASSERT(begin == getBegin(r010));
    TEST_ASSERT(begin == getBegin(r011));
    TEST_ASSERT(begin == getBegin(r012));
    TEST_ASSERT(begin == getBegin(r100));
    TEST_ASSERT(begin == getBegin(r101));
    TEST_ASSERT(begin == getBegin(r102));
    TEST_ASSERT(begin == getBegin(r110));
    TEST_ASSERT(begin == getBegin(r111));
    TEST_ASSERT(begin == getBegin(r112));

    // Test non-const reference path
    Range<PtrType, NotPresent, NotPresent, NotPresent> tmp = {begin};
    PtrType& begin2 = getBegin(tmp);
    assert(begin2 == begin);
  }

  void testGetEnd() {
    TEST_ASSERT(end == getEnd(r100));
    TEST_ASSERT(end == getEnd(r101));
    TEST_ASSERT(end == getEnd(r102));
    TEST_ASSERT(end == getEnd(r110));
    TEST_ASSERT(end == getEnd(r111));
    TEST_ASSERT(end == getEnd(r112));

    // Test non-const reference path
    Range<PtrType, Present, NotPresent, NotPresent> tmp = {begin, end};
    PtrType& end2 = getEnd(tmp);
    assert(end2 == end);

    NotPresent tmp2 = getEnd(r000);
    (void)tmp2;
  }

  void testGetCount() {
    TEST_ASSERT(count == getCount(r010));
    TEST_ASSERT(count == getCount(r011));
    TEST_ASSERT(count == getCount(r012));
    TEST_ASSERT(count == getCount(r110));
    TEST_ASSERT(count == getCount(r111));
    TEST_ASSERT(count == getCount(r112));

    // Test non-const reference path
    Range<PtrType, Present, Present, NotPresent> tmp = {begin, end, count};
    CountType<PtrType>& count2 = getCount(tmp);
    assert(count2 == count);

    NotPresent tmp2 = getCount(r000);
    (void)tmp2;
  }

  void testGetPredicate() {
    getPredicate(r001);
    TEST_ASSERT(!getPredicate(r002).state);
    getPredicate(r011);
    TEST_ASSERT(!getPredicate(r012).state);
    getPredicate(r101);
    TEST_ASSERT(!getPredicate(r102).state);
    getPredicate(r111);
    TEST_ASSERT(!getPredicate(r112).state);

    NotPresent tmp2 = getPredicate(r000);
    (void)tmp2;
  }

  void testAddEnd() {
    TEST_ASSERT(end == getEnd(addEnd(r000, end)));
    TEST_ASSERT(end == getEnd(addEnd(r001, end)));
    TEST_ASSERT(end == getEnd(addEnd(r002, end)));

    TEST_ASSERT(end == getEnd(addEnd(r010, end)));
    TEST_ASSERT(end == getEnd(addEnd(r011, end)));
    TEST_ASSERT(end == getEnd(addEnd(r012, end)));
  }

  void testAddCount() {
    constexpr int x = 123;

    TEST_ASSERT(x == getCount(addCount(r000, x)));
    TEST_ASSERT(x == getCount(addCount(r001, x)));
    TEST_ASSERT(x == getCount(addCount(r002, x)));

    TEST_ASSERT(x == getCount(addCount(r100, x)));
    TEST_ASSERT(x == getCount(addCount(r101, x)));
    TEST_ASSERT(x == getCount(addCount(r102, x)));
  }

  void testAddPredicate() {
    getPredicate(addPredicate(r000, StatelessFalsePredicate{}));
    TEST_ASSERT(!getPredicate(addPredicate(r000, statefulState)).state);

    getPredicate(addPredicate(r010, StatelessFalsePredicate{}));
    TEST_ASSERT(!getPredicate(addPredicate(r010, statefulState)).state);

    getPredicate(addPredicate(r100, StatelessFalsePredicate{}));
    TEST_ASSERT(!getPredicate(addPredicate(r100, statefulState)).state);

    getPredicate(addPredicate(r110, StatelessFalsePredicate{}));
    TEST_ASSERT(!getPredicate(addPredicate(r110, statefulState)).state);
  }

  void testRemoveEnd() {
    {
      auto tmp = removeEnd(r100);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removeEnd(r012);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }
  }

  void testRemoveCount() {
    {
      auto tmp = removeCount(r010);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removeCount(r102);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }
  }

  void testRemovePredicate() {
    {
      auto tmp = removePredicate(r011);
      NotPresent tmp2 = getPredicate(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removePredicate(r012);
      NotPresent tmp2 = getPredicate(tmp);
      (void)tmp2;
    }

    {
      auto tmp = removePredicate(r110);
      NotPresent tmp2 = getPredicate(tmp);
      (void)tmp2;
    }
  }

  template<typename T>
  void testAddConstantTimeEndImpl(T x) {
      auto tmp = addConstantTimeEnd(x);
      assert(getBegin(x) == getBegin(tmp));
      assert(end == getEnd(tmp));
      assert(getCount(x) == getCount(tmp));
      assert(getPredicate(x) == getPredicate(tmp));
  }

  template<typename T>
  void testAddConstantTimeEndNotPossibleImpl(T x) {
      auto tmp = addConstantTimeEnd(x);
      assert(getBegin(x) == getBegin(tmp));
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
      assert(getCount(x) == getCount(tmp));
      assert(getPredicate(x) == getPredicate(tmp));
  }

  void testAddConstantTimeEnd() {
    testAddConstantTimeEndImpl(r010);
    testAddConstantTimeEndImpl(r100);
    testAddConstantTimeEndImpl(r110);
    testAddConstantTimeEndImpl(r111);
    testAddConstantTimeEndImpl(r112);

    testAddConstantTimeEndNotPossibleImpl(r000);
    testAddConstantTimeEndNotPossibleImpl(r001);
    testAddConstantTimeEndNotPossibleImpl(r002);
    testAddConstantTimeEndNotPossibleImpl(r011);
    testAddConstantTimeEndNotPossibleImpl(r012);

    testAddConstantTimeEndNotPossibleImpl(makeRange(slist.begin(), NotPresent{}, 20, NotPresent{}));
  }

  template<typename T>
  void testAddConstantTimeCountImpl(T x) {
    auto tmp = addConstantTimeCount(x);
    assert(getBegin(x) == getBegin(tmp));
    assert(getEnd(x) == getEnd(tmp));
    assert(count == getCount(tmp));
    assert(getPredicate(x) == getPredicate(tmp));
  }

  template<typename T>
  void testAddConstantTimeCountNotPossibleImpl(T x) {
      auto tmp = addConstantTimeCount(x);
      assert(getBegin(x) == getBegin(tmp));
      assert(getEnd(x) == getEnd(tmp));
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
      assert(getPredicate(x) == getPredicate(tmp));
  }

  void testAddConstantTimeCount() {
    testAddConstantTimeCountImpl(r100);
    testAddConstantTimeCountImpl(r110);
    testAddConstantTimeCountImpl(r010);

    testAddConstantTimeCountNotPossibleImpl(r000);
    testAddConstantTimeCountNotPossibleImpl(r001);
    testAddConstantTimeCountNotPossibleImpl(r002);

    testAddConstantTimeCountNotPossibleImpl(r101);
    testAddConstantTimeCountNotPossibleImpl(r102);
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
    testAddLinearTimeEndImpl(r010);
    testAddLinearTimeEndImpl(r011);
    testAddLinearTimeEndImpl(r012);

    //Can't call r000 or r001 or r002 as these don't have a termination condition.

    // End not actually calculated for these
    testAddLinearTimeEndNoCountCheckImpl(r100);
    testAddLinearTimeEndNoCountCheckImpl(r101);
    testAddLinearTimeEndNoCountCheckImpl(r102);

    // Count already present for these
    testAddLinearTimeEndImpl(r110);
    testAddLinearTimeEndImpl(r111);
    testAddLinearTimeEndImpl(r112);

    {
      auto tmp = addLinearTimeEnd(makeRange(slist.begin(), NotPresent{}, 20, NotPresent{}));
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
    // Can't call for r000 or r001, r002 as these don't have a termiantion condition.
    testAddLinearTimeCountImpl(r100);
    testAddLinearTimeCountImpl(r101);
    testAddLinearTimeCountImpl(r102);

    // end not actually calculated for these
    testAddLinearTimeCountNoEndCheckImpl(r010);
    testAddLinearTimeCountNoEndCheckImpl(r011);
    testAddLinearTimeCountNoEndCheckImpl(r012);

    // End not actually calculated
    testAddLinearTimeCountImpl(r110);
    testAddLinearTimeCountImpl(r111);
    testAddLinearTimeCountImpl(r112);

    {
      auto tmp = addLinearTimeCount(makeRange(slist.begin(), slist.end(), NotPresent{}, NotPresent{}));
      assert(20 == getCount(tmp));
      assert(slist.end() == getEnd(tmp));
    }
  }

  void testIsEmpty() {
    assert(!isEmpty(r000));
    assert(!isEmpty(r001));
    assert(!isEmpty(r002));

    assert(!isEmpty(r010));
    assert(!isEmpty(r011));
    assert(!isEmpty(r012));

    assert(!isEmpty(r100));
    assert(!isEmpty(r101));
    assert(!isEmpty(r102));

    assert(!isEmpty(r110));
    assert(!isEmpty(r111));
    assert(!isEmpty(r112));
    
    {
      auto tmp = makeRange(begin, begin, NotPresent{}, NotPresent{});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 0, NotPresent{});
      assert(isEmpty(tmp));
    }

    {
      // Demonstrates that count is used in preference to begin/end pair
      auto tmp = makeRange(begin, end, 0, NotPresent{});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, NotPresent{}, StatefulPredicate{true});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, NotPresent{}, StatelessTruePredicate{});
      assert(isEmpty(tmp));
    }

    // Demonstrate that the begin/end pair or else count is used before reference to
    // the predicate
    {
      auto tmp = makeRange(begin, end, NotPresent{}, StatelessFalsePredicate{});
      assert(!isEmpty(tmp));
      getEnd(tmp) = begin;
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 10, StatelessFalsePredicate{});
      assert(!isEmpty(tmp));
      getCount(tmp) = 0;
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, end, NotPresent{}, StatefulPredicate{false});
      assert(!isEmpty(tmp));
      getEnd(tmp) = begin;
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 10, StatefulPredicate{false});
      assert(!isEmpty(tmp));
      getCount(tmp) = 0;
      assert(isEmpty(tmp));
    }

    // Demonstrate that predicate is eventually used
    {
      auto tmp = makeRange(begin, end, NotPresent{}, StatelessTruePredicate{});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 10, StatelessTruePredicate{});
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, end, NotPresent{}, StatefulPredicate{false});
      assert(!isEmpty(tmp));
      getPredicate(tmp).state = true;
      assert(isEmpty(tmp));
    }

    {
      auto tmp = makeRange(begin, NotPresent{}, 10, StatefulPredicate{false});
      assert(!isEmpty(tmp));
      getPredicate(tmp).state = true;
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

  struct EndOfArrayStateless {
    constexpr bool operator()(PtrType x) const { return x == end; }
  };

  struct EndOfArrayStateful {
    PtrType theEnd;
    constexpr bool operator()(PtrType x) const { return x == theEnd; }
  };

  void testNext() {
    testBoundedRange(makeRange(begin, end, NotPresent{}, NotPresent{}));
    testBoundedRange(makeRange(begin, NotPresent{}, 20, NotPresent{}));
    testBoundedRange(makeRange(begin, end, 20, NotPresent{}));

    testBoundedRange(makeRange(begin, NotPresent{}, NotPresent{}, EndOfArrayStateless{}));
    testBoundedRange(makeRange(begin, NotPresent{}, NotPresent{}, EndOfArrayStateful{end}));

    testBoundedRange(makeRange(begin, end, NotPresent{}, EndOfArrayStateless{}));
    testBoundedRange(makeRange(begin, end, NotPresent{}, EndOfArrayStateful{end}));

    testBoundedRange(makeRange(begin, NotPresent{}, 20, EndOfArrayStateless{}));
    testBoundedRange(makeRange(begin, NotPresent{}, 20, EndOfArrayStateful{end}));

    testBoundedRange(makeRange(begin, end, 20, EndOfArrayStateless{}));
    testBoundedRange(makeRange(begin, end, 20, EndOfArrayStateful{end}));
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
  testGetPredicate();

  testAddEnd();
  testAddCount();
  testAddPredicate();

  testRemoveEnd();
  testRemoveCount();
  testRemovePredicate();

  testAddConstantTimeEnd();
  testAddConstantTimeCount();

  testAddLinearTimeEnd();
  testAddLinearTimeCount();

  testIsEmpty();
  testNext();
}
