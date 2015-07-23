#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "SphericalObject.h"

TEST_GROUP(SphericalObject) {
  SphericalObject getSomeSphericalObject() {
    static int c = 0;
    ++c;
    SphericalObject so;
    so._mass = 1.11 * c;
    so._position = Vector(5.5 * c, 6.6 * c, 7.7 * c);
    so._velocity =  Vector(-2.21 * c, -3.6 * c, 5.01 * c);
    so._radius = 71.3 * c;
    return so;
  }
};

TEST(SphericalObject, Copy) {
  SphericalObject so1 = getSomeSphericalObject();
  PhysicalObject *po2;
  po2 = so1.copy();
  CHECK(so1.equals(*po2));
  delete po2;
}

TEST(SphericalObject, CopyConstructor) {
  SphericalObject so1;
  so1._mass = 221.11;
  so1._position = Vector(225.5, -26.6, 227.7);
  so1._velocity =  Vector(255.21, -43.6, 53.01);
  so1._radius = 871.3;
  SphericalObject so2(so1);
  CHECK(so1.equals(so2));
}

TEST(SphericalObject, CopyFrom) {
  SphericalObject so1 = getSomeSphericalObject();
  SphericalObject so2 = getSomeSphericalObject();
  so1.copyFrom(so2);
  CHECK(so1.equals(so2));
}

TEST(SphericalObject, CopyDifferentTypes) {
  // TODO: chek failed assertion
}
