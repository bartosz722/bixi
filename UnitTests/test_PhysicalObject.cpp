#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "PhysicalObject.h"

TEST_GROUP(PhysicalObject) {
  PhysicalObject * createPhysicalObject() {
    return new PhysicalObject();
  }
};

TEST(PhysicalObject, GetGravityForce) {
  PhysicalObject * pho1 = createPhysicalObject();
  PhysicalObject * pho2 = createPhysicalObject();

  pho1->_mass = 8 * pow(10, 10);
  pho2->_mass = 11 * pow(10, 10);

  pho1->_position = Vector(1000, 1000, 1000);
  pho2->_position = Vector(2000, 2000, 2000);

  Vector gForce = pho1->getGravityForce(*pho2);
  const double gfx = 113025.5374;
  CHECK(vectorsEqual(gForce, Vector(gfx, gfx, gfx), 0.00005));
  DOUBLES_EQUAL(195765.9733, gForce.length(), 0.00005);

  delete pho1;
  delete pho2;
}

TEST(PhysicalObject, GetGravityForce2) {
  PhysicalObject * earth = createPhysicalObject();
  PhysicalObject * man = createPhysicalObject();
  earth->_mass = 5.97219 * pow(10, 24);
  man->_mass = 70;
  man->_position = Vector(6371000, 0, 0);
  Vector forceOnManV = man->getGravityForce(*earth);
  double forceOnMan = forceOnManV.length();
  double g = forceOnMan / man->_mass;
  DOUBLES_EQUAL(9.81961, g, 0.000005);
  delete earth;
  delete man;
}

TEST(PhysicalObject, Copy) {
  PhysicalObject po1, *po2;
  po1._mass = 1.11;
  po1._position = Vector(5.5, 6.6, 7.7);
  po1._speed =  Vector(15.5, 16.6, 17.7);
  po2 = po1.copy();
  CHECK(po1.equals(*po2));
  delete po2;
}

TEST(PhysicalObject, CopyConstructor) {
  PhysicalObject po1;
  po1._mass = 1.11;
  po1._position = Vector(5.5, 6.6, 7.7);
  po1._speed =  Vector(15.5, 16.6, 17.7);
  PhysicalObject po2(po1);
  CHECK(po1.equals(po2));
}

TEST(PhysicalObject, CopyFrom) {
  PhysicalObject po1, po2;
  po1._mass = 1.11;
  po1._position = Vector(5.5, 6.6, 7.7);
  po1._speed =  Vector(15.5, 16.6, 17.7);
  po2._mass = -771.11;
  po2._position = Vector(-885.5, -996.6, -117.7);
  po2._speed =  Vector(1005.05, 1116.16, 1227.27);
  po1.copyFrom(po2);
  CHECK(po1.equals(po2));
}

TEST(PhysicalObject, CopyDifferentTypes) {
  // TODO: chek failed assertion
}
