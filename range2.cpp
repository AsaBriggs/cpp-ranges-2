#include "range2.h"

#include <functional>
#include <cassert>

namespace range2 {
namespace {

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
  Range<int*, NotPresent, NotPresent, NotPresent> r000 = {nullptr};

  Range<int*, NotPresent, NotPresent, std::less<int*>> r001 = {nullptr};
  Range<int*, NotPresent, NotPresent, StatefulPredicate> r002 = {nullptr, {}};

  Range<int*, NotPresent, Present, NotPresent> r010 = {nullptr, 0u};
  Range<int*, NotPresent, Present, std::less<int*>> r011 = {nullptr, 0u};
  Range<int*, NotPresent, Present, StatefulPredicate> r012 = {nullptr, 0u, {}};

  Range<int*, Present, NotPresent, NotPresent> r100 = {nullptr, nullptr};

  Range<int*, Present, NotPresent, std::less<int*>> r101 = {nullptr, nullptr};
  Range<int*, Present, NotPresent, StatefulPredicate> r102 = {nullptr, nullptr, {}};

  Range<int*, Present, Present, NotPresent> r110 = {nullptr, nullptr, 0u};
  Range<int*, Present, Present, std::less<int*>> r111 = {nullptr, nullptr, 0u};
  Range<int*, Present, Present, StatefulPredicate> r112 = {nullptr, nullptr, 0u, {}};

  void testEquality()
  {
    assert(r000 == r000);
    assert(r001 == r001);
    assert(r002 == r002);

    assert(r010 == r010);
    assert(r011 == r011);
    assert(r012 == r012);

    assert(r100 == r100);
    assert(r101 == r101);
    assert(r102 == r102);

    assert(r110 == r110);
    assert(r111 == r111);
    assert(r112 == r112);

  }

  void testInequality()
  {
    assert(!(r000 != r000));
    assert(!(r001 != r001));
    assert(!(r002 != r002));

    assert(!(r010 != r010));
    assert(!(r011 != r011));
    assert(!(r012 != r012));

    assert(!(r100 != r100));
    assert(!(r101 != r101));
    assert(!(r102 != r102));

    assert(!(r110 != r110));
    assert(!(r111 != r111));
    assert(!(r112 != r112));

  }

  void testGetBegin()
  {
    assert(nullptr == getBegin(r000));
    assert(nullptr == getBegin(r001));
    assert(nullptr == getBegin(r002));
    assert(nullptr == getBegin(r010));
    assert(nullptr == getBegin(r011));
    assert(nullptr == getBegin(r012));
    assert(nullptr == getBegin(r100));
    assert(nullptr == getBegin(r101));
    assert(nullptr == getBegin(r102));
    assert(nullptr == getBegin(r110));
    assert(nullptr == getBegin(r111));
    assert(nullptr == getBegin(r112));
  }

  void testGetEnd()
  {
    assert(nullptr == getEnd(r100));
    assert(nullptr == getEnd(r101));
    assert(nullptr == getEnd(r102));
    assert(nullptr == getEnd(r110));
    assert(nullptr == getEnd(r111));
    assert(nullptr == getEnd(r112));
  }

  void testGetCount()
  {
    assert(0u == getCount(r010));
    assert(0u == getCount(r011));
    assert(0u == getCount(r012));
    assert(0u == getCount(r110));
    assert(0u == getCount(r111));
    assert(0u == getCount(r112));
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

    assert(&x == getEnd(addEnd(r000, &x)));
    assert(&x == getEnd(addEnd(r001, &x)));
    assert(&x == getEnd(addEnd(r002, &x)));

    assert(&x == getEnd(addEnd(r010, &x)));
    assert(&x == getEnd(addEnd(r011, &x)));
    assert(&x == getEnd(addEnd(r012, &x)));
  }

  void testAddCount()
  {
    int x = 123;

    assert(x == getCount(addCount(r000, x)));
    assert(x == getCount(addCount(r001, x)));
    assert(x == getCount(addCount(r002, x)));

    assert(x == getCount(addCount(r100, x)));
    assert(x == getCount(addCount(r101, x)));
    assert(x == getCount(addCount(r102, x)));
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
