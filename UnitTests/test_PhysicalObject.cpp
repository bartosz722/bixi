#include <cmath>
#include "CppUTest/TestHarness.h"
#include "PhysicalObject.h"
#include "PhysicalDefinitions.h"

TEST_GROUP(PhysicalObject) {
  void setup() {
    c = 0;
  }

  PhysicalObject * getSomePhysicalObject() {
    ++c;
    PhysicalObject * o = new PhysicalObject;;
    o->setId(c);
    o->_mass = 1.11 * c;
    o->_position = Vector(5.5 * c, 6.6 * c, 7.7 * c);
    o->_velocity =  Vector(-2.21 * c, -3.6 * c, 5.01 * c);
    o->_force = Vector(8.09 * c, -1.12 * c, -99.23 *c);
    return o;
  }

  int c;
};

TEST(PhysicalObject, GetGravityForce) {
  PhysicalObject earth;
  earth._mass = 5.972 * pow(10, 24);
  earth._position = Vector(100, 100, 100);

  PhysicalObject man;
  man._mass = 70;
  man._position = Vector(100+6370000, 100, 100);

  const double gForceAbsOK = 1030241344.0 * 10000.0 * constG;

  Vector gForceVect = man.getGravityForce(earth);
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
  CHECK(po._position == Vector(100+2, 200+4, 300+6));
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
