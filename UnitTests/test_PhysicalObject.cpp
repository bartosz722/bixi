#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "PhysicalObject.h"

TEST_GROUP(PhysicalObject) {
  void setup() {
    c = 0;
  }

  PhysicalObject * createPhysicalObject() {
    // TODO: do kasacji po poprawieniu testów GetGravityForce*
    return new PhysicalObject();
  }

  PhysicalObject * getSomePhysicalObject() {
    ++c;
    PhysicalObject * o = new PhysicalObject;;
    o->_mass = 1.11 * c;
    o->_position = Vector(5.5 * c, 6.6 * c, 7.7 * c);
    o->_velocity =  Vector(-2.21 * c, -3.6 * c, 5.01 * c);
    o->_force = Vector(8.09 * c, -1.12 * c, -99.23 *c);
    return o;
  }

  int c;
};

TEST(PhysicalObject, CopyConstructor) {
  PhysicalObject * po = getSomePhysicalObject();
  PhysicalObject po2(*po);
  CHECK_FALSE(po->getId() == po2.getId());
  CHECK(po->getType() == po2.getType());
  CHECK(po->_mass == po2._mass);
  CHECK(po->_position == po2._position);
  CHECK(po->_velocity == po2._velocity);
  CHECK(po->_force == po2._force);
  delete po;
}

TEST(PhysicalObject, GetGravityForce) {
  // TODO uniezależnić od wartości stałej G
  PhysicalObject * pho1 = createPhysicalObject();
  PhysicalObject * pho2 = createPhysicalObject();

  pho1->_mass = 8 * pow(10, 10);
  pho2->_mass = 11 * pow(10, 10);

  pho1->_position = Vector(1000, 1000, 1000);
  pho2->_position = Vector(2000, 2000, 2000);

  Vector gForce = pho1->getGravityForce(*pho2);
  const double gfx = 113025.5374;
  CHECK(gForce.equals(Vector(gfx, gfx, gfx), 0.00005));
  DOUBLES_EQUAL(195765.9733, gForce.length(), 0.00005);

  delete pho1;
  delete pho2;
}

TEST(PhysicalObject, GetGravityForce2) {
  // TODO uniezależnić od wartości stałej G
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
  CHECK(po._position == Vector(100+2, 200+4, 300+6));
}

TEST(PhysicalObject, ClearNextStateVariables) {
  PhysicalObject po;
  po._force = Vector(1, 2, 3);
  po.clearNextStateVariables();
  CHECK(po._force == Vector(0, 0, 0));
}
