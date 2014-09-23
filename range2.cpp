#include "range2.h"

#include <functional>

namespace range2 {
namespace {

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  typedef int const* ptrType;

  struct StatefulPredicate
  {
    bool state;
    bool operator()(ptrType, ptrType) const { return state; }

    friend
    constexpr bool operator==(StatefulPredicate const& x, StatefulPredicate const& y)
    {
      return x.state == y.state;
    }
  };

  constexpr int arr[20] = {1};

  // Test Range construction
  constexpr Range<ptrType, NotPresent, NotPresent, NotPresent> r000 = {&arr[0]};

  constexpr Range<ptrType, NotPresent, NotPresent, std::less<ptrType>> r001 = {&arr[0]};
  constexpr Range<ptrType, NotPresent, NotPresent, StatefulPredicate> r002 = {&arr[0], {true}};

  constexpr Range<ptrType, NotPresent, Present, NotPresent> r010 = {&arr[0], 0u};
  constexpr Range<ptrType, NotPresent, Present, std::less<ptrType>> r011 = {&arr[0], 0u};
  constexpr Range<ptrType, NotPresent, Present, StatefulPredicate> r012 = {&arr[0], 0u, {true}};

  constexpr Range<ptrType, Present, NotPresent, NotPresent> r100 = {&arr[0], &arr[0]};

  constexpr Range<ptrType, Present, NotPresent, std::less<ptrType>> r101 = {&arr[0], &arr[0]};
  constexpr Range<ptrType, Present, NotPresent, StatefulPredicate> r102 = {&arr[0], &arr[0], {true}};

  constexpr Range<ptrType, Present, Present, NotPresent> r110 = {&arr[0], &arr[0], 0u};
  constexpr Range<ptrType, Present, Present, std::less<ptrType>> r111 = {&arr[0], &arr[0], 0u};
  constexpr Range<ptrType, Present, Present, StatefulPredicate> r112 = {&arr[0], &arr[0], 0u, {true}};

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
    TEST_ASSERT(&arr[0] == getBegin(r000));
    TEST_ASSERT(&arr[0] == getBegin(r001));
    TEST_ASSERT(&arr[0] == getBegin(r002));
    TEST_ASSERT(&arr[0] == getBegin(r010));
    TEST_ASSERT(&arr[0] == getBegin(r011));
    TEST_ASSERT(&arr[0] == getBegin(r012));
    TEST_ASSERT(&arr[0] == getBegin(r100));
    TEST_ASSERT(&arr[0] == getBegin(r101));
    TEST_ASSERT(&arr[0] == getBegin(r102));
    TEST_ASSERT(&arr[0] == getBegin(r110));
    TEST_ASSERT(&arr[0] == getBegin(r111));
    TEST_ASSERT(&arr[0] == getBegin(r112));
  }

  void testGetEnd()
  {
    TEST_ASSERT(&arr[0] == getEnd(r100));
    TEST_ASSERT(&arr[0] == getEnd(r101));
    TEST_ASSERT(&arr[0] == getEnd(r102));
    TEST_ASSERT(&arr[0] == getEnd(r110));
    TEST_ASSERT(&arr[0] == getEnd(r111));
    TEST_ASSERT(&arr[0] == getEnd(r112));
  }

  void testGetCount()
  {
    TEST_ASSERT(0u == getCount(r010));
    TEST_ASSERT(0u == getCount(r011));
    TEST_ASSERT(0u == getCount(r012));
    TEST_ASSERT(0u == getCount(r110));
    TEST_ASSERT(0u == getCount(r111));
    TEST_ASSERT(0u == getCount(r112));
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
  }

  void testAddEnd()
  {
    TEST_ASSERT(&arr[10] == getEnd(addEnd(r000, &arr[10])));
    TEST_ASSERT(&arr[10] == getEnd(addEnd(r001, &arr[10])));
    TEST_ASSERT(&arr[10] == getEnd(addEnd(r002, &arr[10])));

    TEST_ASSERT(&arr[10] == getEnd(addEnd(r010, &arr[10])));
    TEST_ASSERT(&arr[10] == getEnd(addEnd(r011, &arr[10])));
    TEST_ASSERT(&arr[10] == getEnd(addEnd(r012, &arr[10])));
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
    getPredicate(addPredicate(r000, std::less<ptrType>()));
    TEST_ASSERT(getPredicate(addPredicate(r000, StatefulPredicate{true})).state);

    getPredicate(addPredicate(r010, std::less<ptrType>()));
    TEST_ASSERT(getPredicate(addPredicate(r010, StatefulPredicate{true})).state);

    getPredicate(addPredicate(r100, std::less<ptrType>()));
    TEST_ASSERT(getPredicate(addPredicate(r100, StatefulPredicate{true})).state);

    getPredicate(addPredicate(r110, std::less<ptrType>()));
    TEST_ASSERT(getPredicate(addPredicate(r110, StatefulPredicate{true})).state);
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
}
