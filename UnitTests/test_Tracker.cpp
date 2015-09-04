#include <cmath>
#include "CppUTest/TestHarness.h"
#include "Tracker.h"

TEST_GROUP(Tracker) {
  const Tracker::Track * getTrack(const Tracker & t, int id) {
    auto fi = t.getTracks().find(id);
    if(fi == t.getTracks().end()) {
      return nullptr;
    }
    return &(fi->second);
  }
};

TEST(Tracker, PushAndGetData_Density) {
  const int density = 3;
  const int capacity = 10;

  PhysicalObjectsContainer poc;
  for(int i=0; i<2; ++i) {
    PhysicalObject po;
    po.setId(i+1);
    po._position = Vector(i+1, 0, 0);
    poc.insert(po);
  }

  Tracker t(density, capacity);
  t.trackObject(2);

  // Insert data
  for(int i=0; i<density*(capacity-1); ++i) {
    for(auto & po : poc) {
      po->_position.v[1] = i;
    }
    t.pushData(poc);

    auto track = getTrack(t, 2);
    CHECK(track != nullptr);
    CHECK_EQUAL(1 + i/density, track->size());
  }

  CHECK(getTrack(t, 1) == nullptr); // don't track it

  // Check data
  auto track = getTrack(t, 2);
  CHECK(track != nullptr);
  int i = 0;
  for(const auto & position : *track) {
    CHECK(position == Vector(2, i*density, 0));
    ++i;
  }
}

TEST(Tracker, PushAndGetDataMoreTimesThanCapacity) {
  // Not needed, tests in CyclicBuffer do these checks.
}

TEST(Tracker, Capacity) {
  const int density = 3;
  const int capacity = 20;

  PhysicalObjectsContainer poc;
  PhysicalObject po;
  po.setId(1);
  poc.insert(po);

  Tracker t(density, capacity);
  t.trackObject(1);

  for(int i=0; i<density*capacity*2; ++i) {
    t.pushData(poc);
    auto track = getTrack(t, 1);
    CHECK(track != nullptr);
    if(i < (capacity-1)*density) {
      CHECK(track->size() < capacity);
    }
    else {
      CHECK(track->size() == capacity);
    }
  }
}

TEST(Tracker, DoNotTrackInactiveObject) {
  PhysicalObjectsContainer poc;
  PhysicalObject po;
  po.setId(1);
  poc.insert(po);

  Tracker t(1, 20);
  t.trackObject(1);

  // Insert data
  for(int i=0; i<10; ++i) {
    if(i == 5) {
      (*poc.begin())->_active = false;
    }
    t.pushData(poc);
  }

  auto track = getTrack(t, 1);
  CHECK(track != nullptr);
  CHECK_EQUAL(5, track->size());
}

TEST(Tracker, GetSizeOfLongestTrack) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<3; ++i) {
    PhysicalObject po;
    po.setId(i);
    po._position = Vector(i+1, 0, 0);
    poc.insert(po);
  }

  Tracker t(1, 20);
  t.trackObject(0);
  t.trackObject(1);

  CHECK_EQUAL(0, t.getSizeOfLongestTrack());

  // Insert data
  for(int i=0; i<10; ++i) {
    if(i == 2) {
      poc[0]->_active = false;
    }
    if(i == 5) {
      poc[1]->_active = false;
    }

    t.pushData(poc);
  }

  CHECK_EQUAL(5, t.getSizeOfLongestTrack());
}
