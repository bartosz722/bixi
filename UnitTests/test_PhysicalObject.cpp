#include <cmath>
#include "CppUTest/TestHarness.h"
#include "PhysicalObject.h"
#include "PhysicalDefinitions.h"
#include "unit_tests_tools.h"

TEST_GROUP(PhysicalObject) {
  void checkObjectsEqual(const PhysicalObject & po1, const PhysicalObject & po2) {
    CHECK(po1.getType() == po2.getType());
    CHECK(po1.getId() == po2.getId());
    CHECK(po1._active == po2._active);
    CHECK(po1._mass == po2._mass);
    CHECK(po1._velocity == po2._velocity);
    CHECK(po1._position == po2._position);
    CHECK(po1._force == po2._force);
    CHECK(po1._direction == po2._direction);
    CHECK(po1._orientation == po2._orientation);
  }
};

TEST(PhysicalObject, Init) {
  PhysicalObject po;
  CHECK(PhysicalObjectType::PhysicalObject == po.getType());
  CHECK_EQUAL(PhysicalObject::_invalidId, po.getId());
  CHECK(po._active);
}

TEST(PhysicalObject, GetGravityForce) {
  PhysicalObject earth;
  earth._mass = 5.972 * pow(10, 24);
  earth._position = Vector(100, 100, 100);

  PhysicalObject man;
  man._mass = 70;
  man._position = Vector(100+6370000, 100, 100);

  const double gForceAbsOK = 1030241344.0 * 10000.0 * constG;

  Vector gForceVect = man.getGravityForce(earth, constG);
  CHECK(gForceVect.equals(Vector(-gForceAbsOK, 0, 0), gForceAbsOK * 0.000000001));
}

TEST(PhysicalObject, DistanceTo) {
  PhysicalObject po1, po2;
  po1._position = Vector(1, 2, 3);
  po2._position = Vector(6, 7, 8);
  Vector dist = po1.distanceTo(po2);
  CHECK(dist == Vector(-5, -5, -5));
}

TEST(PhysicalObject, moveToNextState) {
  PhysicalObject po;
  po._mass = 5;
  po._velocity = Vector(1, 2, 3);
  po._position = Vector(100, 200, 300);
  po._force = Vector(10, 20, 30);

  po.moveToNextState(1);
  CHECK(po._velocity == Vector(1+2, 2+4, 3+6));
  CHECK(po._position == Vector(100+3, 200+6, 300+9));
}

TEST(PhysicalObject, ClearNextStateVariables) {
  PhysicalObject po;
  po._force = Vector(1, 2, 3);
  po.clearNextStateVariables();
  CHECK(po._force == Vector(0, 0, 0));
}

TEST(PhysicalObject, GetSetId) {
  PhysicalObject po;
  po.setId(77);
  CHECK_EQUAL(77, po.getId());
}

TEST(PhysicalObject, Copy) {
  PhysicalObject po;
  po.setId(11);
  po._mass = 5;
  po._velocity = Vector(1, 2, 3);
  po._position = Vector(100, 200, 300);
  po._force = Vector(10, 20, 30);

  std::unique_ptr<PhysicalObject> cp = po.copy();
  checkObjectsEqual(po, *cp);
}

TEST(PhysicalObject, CopyFrom) {
  PhysicalObject po, po2;
  po.setId(11);
  po._mass = 5;
  po._velocity = Vector(1, 2, 3);
  po._position = Vector(100, 200, 300);
  po._force = Vector(10, 20, 30);

  po2.copyFrom(po);
  checkObjectsEqual(po, po2);
}

TEST(PhysicalObject, TurnFromLeftToRight) {
  const double tolerance = 0.000001;

  {
    PhysicalObject po;
    po._direction = {1, 0, 0};
    po._orientation = {0, 1, 0};
    po.turnFromLeftToRight(90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(0, 0, 1), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(0, 1, 0), tolerance));
  }

  {
    PhysicalObject po;
    po._direction = {1, 0, 0};
    po._orientation = {0, 1, 0};
    po.turnFromLeftToRight(-90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(0, 0, -1), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(0, 1, 0), tolerance));
  }

  {
    PhysicalObject po;
    po._direction = {0, 0, -1};
    po._orientation = {-1, 0, 0};
    po.turnFromLeftToRight(90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(0, 1, 0), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(-1, 0, 0), tolerance));
  }
}

TEST(PhysicalObject, TurnFromDownToUp) {
  const double tolerance = 0.000001;

  {
    PhysicalObject po;
    po._direction = {1, 0, 0};
    po._orientation = {0, 1, 0};
    po.turnFromDownToUp(90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(0, 1, 0), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(-1, 0, 0), tolerance));
  }

  {
    PhysicalObject po;
    po._direction = {1, 0, 0};
    po._orientation = {0, 1, 0};
    po.turnFromDownToUp(-90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(0, -1, 0), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(1, 0, 0), tolerance));
  }

  {
    PhysicalObject po;
    po._direction = {0, 0, -1};
    po._orientation = {-1, 0, 0};
    po.turnFromDownToUp(90);
    CHECK(ut_tools::compareVectors(po._direction, Vector(-1, 0, 0), tolerance));
    CHECK(ut_tools::compareVectors(po._orientation, Vector(0, 0, 1), tolerance));
  }
}
