#include <cmath>
#include "CppUTest/TestHarness.h"
#include "Tracker.h"

TEST_GROUP(Tracker) {
};

TEST(Tracker, PushAndGetData_Density) {
  const int density = 3;
  const int capacity = 10;

  PhysicalObjectsContainer poc;
  PhysicalObject po;
  for(int i=0; i<2; ++i) {
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

    auto track = t.getTrack(2);
    CHECK(track != nullptr);
    CHECK_EQUAL(1 + i/density, track->size());
  }

  CHECK(t.getTrack(1) == nullptr);

  // Check data
  auto track = t.getTrack(2);
  CHECK(track != nullptr);
  int i = 0;
  for(const auto & position : *track) {
    CHECK(position == Vector(2, i*density, 0));
    ++i;
  }
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
    auto track = t.getTrack(1);
    CHECK(track != nullptr);
    CHECK(track->size() <= capacity);
  }
}
