#include "range2.h"

#include <cassert>

namespace range2 {
namespace {

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  typedef int const* ptrType;

  struct StatelessPredicate
  {
    constexpr bool operator()(ptrType) const { return true; }

    friend
    constexpr bool operator==(StatelessPredicate const& x, StatelessPredicate const& y)
    {
      return true;
    }
  };

  struct StatefulPredicate
  {
    bool state;
    constexpr bool operator()(ptrType) const { return state; }

    friend
    constexpr bool operator==(StatefulPredicate const& x, StatefulPredicate const& y)
    {
      return x.state == y.state;
    }
  };

  constexpr int arr[20] = {1};

  constexpr ptrType begin = &arr[0];
  constexpr ptrType end = &arr[10];
  constexpr int count = 10;
  constexpr StatefulPredicate statefulState = {true};

  // Test Range construction
  constexpr Range<ptrType, NotPresent, NotPresent, NotPresent> r000 = {begin};

  constexpr Range<ptrType, NotPresent, NotPresent, StatelessPredicate> r001 = {begin};
  constexpr Range<ptrType, NotPresent, NotPresent, StatefulPredicate> r002 = {begin, statefulState};

  constexpr Range<ptrType, NotPresent, Present, NotPresent> r010 = {begin, count};
  constexpr Range<ptrType, NotPresent, Present, StatelessPredicate> r011 = {begin, count};
  constexpr Range<ptrType, NotPresent, Present, StatefulPredicate> r012 = {begin, count, statefulState};

  constexpr Range<ptrType, Present, NotPresent, NotPresent> r100 = {begin, end};

  constexpr Range<ptrType, Present, NotPresent, StatelessPredicate> r101 = {begin, end};
  constexpr Range<ptrType, Present, NotPresent, StatefulPredicate> r102 = {begin, end, statefulState};

  constexpr Range<ptrType, Present, Present, NotPresent> r110 = {begin, end, count};
  constexpr Range<ptrType, Present, Present, StatelessPredicate> r111 = {begin, end, count};
  constexpr Range<ptrType, Present, Present, StatefulPredicate> r112 = {begin, end, count, statefulState};

  void testEquality()
  {
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

  void testInequality()
  {
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

  void testGetBegin()
  {
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
    Range<ptrType, NotPresent, NotPresent, NotPresent> tmp = {begin};
    ptrType& begin2 = getBegin(tmp);
    assert(begin2 == begin);
  }

  void testGetEnd()
  {
    TEST_ASSERT(end == getEnd(r100));
    TEST_ASSERT(end == getEnd(r101));
    TEST_ASSERT(end == getEnd(r102));
    TEST_ASSERT(end == getEnd(r110));
    TEST_ASSERT(end == getEnd(r111));
    TEST_ASSERT(end == getEnd(r112));

    // Test non-const reference path
    Range<ptrType, Present, NotPresent, NotPresent> tmp = {begin, end};
    ptrType& end2 = getEnd(tmp);
    assert(end2 == end);

    NotPresent tmp2 = getEnd(r000);
    (void)tmp2;
  }

  void testGetCount()
  {
    TEST_ASSERT(count == getCount(r010));
    TEST_ASSERT(count == getCount(r011));
    TEST_ASSERT(count == getCount(r012));
    TEST_ASSERT(count == getCount(r110));
    TEST_ASSERT(count == getCount(r111));
    TEST_ASSERT(count == getCount(r112));

    // Test non-const reference path
    Range<ptrType, Present, Present, NotPresent> tmp = {begin, end, count};
    CountType<ptrType>& count2 = getCount(tmp);
    assert(count2 == count);

    NotPresent tmp2 = getCount(r000);
    (void)tmp2;
  }

  void testGetPredicate()
  {
    getPredicate(r001);
    TEST_ASSERT(getPredicate(r002).state);
    getPredicate(r011);
    TEST_ASSERT(getPredicate(r012).state);
    getPredicate(r101);
    TEST_ASSERT(getPredicate(r102).state);
    getPredicate(r111);
    TEST_ASSERT(getPredicate(r112).state);

    NotPresent tmp2 = getPredicate(r000);
    (void)tmp2;
  }

  void testAddEnd()
  {
    TEST_ASSERT(end == getEnd(addEnd(r000, end)));
    TEST_ASSERT(end == getEnd(addEnd(r001, end)));
    TEST_ASSERT(end == getEnd(addEnd(r002, end)));

    TEST_ASSERT(end == getEnd(addEnd(r010, end)));
    TEST_ASSERT(end == getEnd(addEnd(r011, end)));
    TEST_ASSERT(end == getEnd(addEnd(r012, end)));
  }

  void testAddCount()
  {
    constexpr int x = 123;

    TEST_ASSERT(x == getCount(addCount(r000, x)));
    TEST_ASSERT(x == getCount(addCount(r001, x)));
    TEST_ASSERT(x == getCount(addCount(r002, x)));

    TEST_ASSERT(x == getCount(addCount(r100, x)));
    TEST_ASSERT(x == getCount(addCount(r101, x)));
    TEST_ASSERT(x == getCount(addCount(r102, x)));
  }

  void testAddPredicate()
  {
    getPredicate(addPredicate(r000, StatelessPredicate()));
    TEST_ASSERT(getPredicate(addPredicate(r000, statefulState)).state);

    getPredicate(addPredicate(r010, StatelessPredicate()));
    TEST_ASSERT(getPredicate(addPredicate(r010, statefulState)).state);

    getPredicate(addPredicate(r100, StatelessPredicate()));
    TEST_ASSERT(getPredicate(addPredicate(r100, statefulState)).state);

    getPredicate(addPredicate(r110, StatelessPredicate()));
    TEST_ASSERT(getPredicate(addPredicate(r110, statefulState)).state);
  }

  void testRemoveEnd()
  {
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

  void testRemoveCount()
  {
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

  void testRemovePredicate()
  {
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

  void testAddConstantTimeEnd()
  {
    {
      auto tmp = addConstantTimeEnd(r010);
      assert(count == getCount(tmp));
      assert(end == getEnd(tmp));
    }

    {
      auto tmp = addConstantTimeEnd(r100);
      assert(end == getEnd(tmp));
    }

    {
      auto tmp = addConstantTimeEnd(r110);
      assert(count == getCount(tmp));
      assert(end == getEnd(tmp));
    }

    {
      auto tmp = addConstantTimeEnd(r000);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }

    {
      auto tmp = addConstantTimeEnd(r001);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }

    {
      auto tmp = addConstantTimeEnd(r002);
      NotPresent tmp2 = getEnd(tmp);
      (void)tmp2;
    }
  }

  void testAddConstantTimeCount()
  {
    {
      auto tmp = addConstantTimeCount(r100);
      assert(count == getCount(tmp));
      assert(end == getEnd(tmp));
    }

    {
      auto tmp = addConstantTimeCount(r010);
      assert(count == getCount(tmp));
    }

    {
      auto tmp = addConstantTimeCount(r110);
      assert(count == getCount(tmp));
      assert(end == getEnd(tmp));
    }

    {
      auto tmp = addConstantTimeCount(r000);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }

    {
      auto tmp = addConstantTimeCount(r001);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }

    {
      auto tmp = addConstantTimeCount(r002);
      NotPresent tmp2 = getCount(tmp);
      (void)tmp2;
    }

  }

  void testEmpty()
  {
    assert(!empty(r000));
    assert(!empty(r001));
    assert(!empty(r002));

    assert(!empty(r010));
    assert(!empty(r011));
    assert(!empty(r012));

    assert(!empty(r100));
    assert(!empty(r101));
    assert(!empty(r102));

    assert(!empty(r110));
    assert(!empty(r111));
    assert(!empty(r112));
  }
} // unnamed namespace
} // namespace range2

using namespace range2;

int main()
{
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

  testEmpty();
}
