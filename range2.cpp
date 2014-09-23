#include "range2.h"

#include <functional>

namespace range2 {
namespace {

#define TEST_ASSERT(x) static_assert(x, "Unexpected")

  struct StatefulPredicate
  {
    bool state;
    bool operator()(int*, int*) const { return state; }

    friend
    constexpr bool operator==(StatefulPredicate const& x, StatefulPredicate const& y)
    {
      return x.state == y.state;
    }
  };

  // Test Range construction
  constexpr Range<int*, NotPresent, NotPresent, NotPresent> r000 = {nullptr};

  constexpr Range<int*, NotPresent, NotPresent, std::less<int*>> r001 = {nullptr};
  constexpr Range<int*, NotPresent, NotPresent, StatefulPredicate> r002 = {nullptr, {}};

  constexpr Range<int*, NotPresent, Present, NotPresent> r010 = {nullptr, 0u};
  constexpr Range<int*, NotPresent, Present, std::less<int*>> r011 = {nullptr, 0u};
  constexpr Range<int*, NotPresent, Present, StatefulPredicate> r012 = {nullptr, 0u, {}};

  constexpr Range<int*, Present, NotPresent, NotPresent> r100 = {nullptr, nullptr};

  constexpr Range<int*, Present, NotPresent, std::less<int*>> r101 = {nullptr, nullptr};
  constexpr Range<int*, Present, NotPresent, StatefulPredicate> r102 = {nullptr, nullptr, {}};

  constexpr Range<int*, Present, Present, NotPresent> r110 = {nullptr, nullptr, 0u};
  constexpr Range<int*, Present, Present, std::less<int*>> r111 = {nullptr, nullptr, 0u};
  constexpr Range<int*, Present, Present, StatefulPredicate> r112 = {nullptr, nullptr, 0u, {}};

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
    TEST_ASSERT(nullptr == getBegin(r000));
    TEST_ASSERT(nullptr == getBegin(r001));
    TEST_ASSERT(nullptr == getBegin(r002));
    TEST_ASSERT(nullptr == getBegin(r010));
    TEST_ASSERT(nullptr == getBegin(r011));
    TEST_ASSERT(nullptr == getBegin(r012));
    TEST_ASSERT(nullptr == getBegin(r100));
    TEST_ASSERT(nullptr == getBegin(r101));
    TEST_ASSERT(nullptr == getBegin(r102));
    TEST_ASSERT(nullptr == getBegin(r110));
    TEST_ASSERT(nullptr == getBegin(r111));
    TEST_ASSERT(nullptr == getBegin(r112));
  }

  void testGetEnd()
  {
    TEST_ASSERT(nullptr == getEnd(r100));
    TEST_ASSERT(nullptr == getEnd(r101));
    TEST_ASSERT(nullptr == getEnd(r102));
    TEST_ASSERT(nullptr == getEnd(r110));
    TEST_ASSERT(nullptr == getEnd(r111));
    TEST_ASSERT(nullptr == getEnd(r112));
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
    getPredicate(r002);
    getPredicate(r011);
    getPredicate(r012);
    getPredicate(r101);
    getPredicate(r102);
    getPredicate(r111);
    getPredicate(r112);
  }

  void testAddEnd()
  {
    int x = 0;

    TEST_ASSERT(&x == getEnd(addEnd(r000, &x)));
    TEST_ASSERT(&x == getEnd(addEnd(r001, &x)));
    TEST_ASSERT(&x == getEnd(addEnd(r002, &x)));

    TEST_ASSERT(&x == getEnd(addEnd(r010, &x)));
    TEST_ASSERT(&x == getEnd(addEnd(r011, &x)));
    TEST_ASSERT(&x == getEnd(addEnd(r012, &x)));
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
    getPredicate(addPredicate(r000, std::less<int*>()));
    getPredicate(addPredicate(r000, StatefulPredicate()));

    getPredicate(addPredicate(r010, std::less<int*>()));
    getPredicate(addPredicate(r010, StatefulPredicate()));

    getPredicate(addPredicate(r100, std::less<int*>()));
    getPredicate(addPredicate(r100, StatefulPredicate()));

    getPredicate(addPredicate(r110, std::less<int*>()));
    getPredicate(addPredicate(r110, StatefulPredicate()));
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
