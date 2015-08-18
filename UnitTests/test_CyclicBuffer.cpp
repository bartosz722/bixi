#include "CppUTest/TestHarness.h"
#include "CyclicBuffer.h"

TEST_GROUP(CyclicBuffer) {
};

TEST(CyclicBuffer, Init) {
  CyclicBuffer<int> b(100);
  CHECK(b.isEmpty());
  CHECK(!b.isFull());
  CHECK_EQUAL(0, b.size());
  CHECK_EQUAL(100, b.capacity());
}

TEST(CyclicBuffer, PushAndIterate) {
  const int capacity = 10;
  CyclicBuffer<int> b(capacity);

  for(int i=0; i<capacity*2; ++i) {
    b.push_back(i);

    CHECK(!b.isEmpty());
    bool full = b.isFull();
    CHECK(i >= (capacity-1) ? full : !full);
    if(!full) {
      CHECK_EQUAL(i+1, b.size());
    }
    else {
      CHECK_EQUAL(capacity, b.size());
    }

    CyclicBuffer<int>::ConstIterator it = b.begin();
    int expectedVal = 0;
    int values = i + 1;
    if(i >= capacity) {
      expectedVal = i - capacity + 1;
      values = capacity;
    }

    for(int k=0; k<values; ++k) {
      CHECK(it != b.end());
      CHECK_EQUAL(expectedVal, *it);
      ++expectedVal;
      ++it;
    }
    CHECK(!(it != b.end()));
  }
}

TEST(CyclicBuffer, IterateEmpty) {
  CyclicBuffer<int> b(100);
  CyclicBuffer<int>::ConstIterator it = b.begin();
  CHECK(!(it != b.end()));
}


