#include "range2.h"

#include <functional>
#include <cassert>

namespace range2 {
namespace {

  struct StatefulPredicate
  {
    bool state;
    bool operator()(int*, int*) const { return state; }
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

  void testGetPred()
  {
    getPred(r001);
    getPred(r002);
    getPred(r011);
    getPred(r012);
    getPred(r101);
    getPred(r102);
    getPred(r111);
    getPred(r112);
  }

} // unnamed namespace
} // namespace range2

  using namespace range2;

int main()
{
  testGetBegin();
  testGetEnd();
  testGetCount();
  testGetPred();
}
