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

  for(int i=0; i<capacity*2 + capacity/3; ++i) {
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

  CHECK_EQUAL(capacity, b.capacity());
}

TEST(CyclicBuffer, IterateEmpty) {
  CyclicBuffer<int> b(100);
  CyclicBuffer<int>::ConstIterator it = b.begin();
  CHECK(!(it != b.end()));
}

TEST(CyclicBuffer, ZeroSizeBuffer) {
  {
    CyclicBuffer<int> b;
    CHECK_EQUAL(0, b.size());
    CHECK_EQUAL(0, b.capacity());
    CHECK(b.isEmpty());
    CHECK(b.isFull());
    CyclicBuffer<int>::ConstIterator it = b.begin();
    CHECK(!(it != b.end()));
  }

  {
    CyclicBuffer<int> b0(0);
    CHECK_EQUAL(0, b0.size());
    CHECK_EQUAL(0, b0.capacity());
    CHECK(b0.isEmpty());
    CHECK(b0.isFull());
    CyclicBuffer<int>::ConstIterator it0 = b0.begin();
    CHECK(!(it0 != b0.end()));
  }
}

TEST(CyclicBuffer, MoveConstructor_MoveOperator) {
  CyclicBuffer<int> b(3);
  b.push_back(1);
  b.push_back(2);

  CyclicBuffer<int> b2(std::move(b));
  CHECK_EQUAL(3, b2.capacity());
  CHECK_EQUAL(2, b2.size());

  CyclicBuffer<int>::ConstIterator it2 = b2.begin();
  for(int i=0; i<2; ++i) {
    CHECK(it2 != b2.end());
    CHECK_EQUAL(i+1, *it2);
    ++it2;
  }

  CyclicBuffer<int> b3(10);
  b3 = std::move(b2);
  CyclicBuffer<int>::ConstIterator it3 = b3.begin();
  for(int i=0; i<2; ++i) {
    CHECK(it3 != b3.end());
    CHECK_EQUAL(i+1, *it3);
    ++it3;
  }
}
