#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "Vector.h"

TEST_GROUP(Vector) {
};

TEST(Vector, Initialization) {
  Vector v0;
  CHECK_EQUAL(0.0, v0.v[0]);
  CHECK_EQUAL(0.0, v0.v[1]);
  CHECK_EQUAL(0.0, v0.v[2]);

  Vector v1(1.1, 2.2, 3.3);
  DOUBLES_EQUAL(1.1, v1.v[0], 0.0);
  DOUBLES_EQUAL(2.2, v1.v[1], 0.0);
  DOUBLES_EQUAL(3.3, v1.v[2], 0.0);
}

TEST(Vector, AddSubtract) {
  Vector v1(1.1, 2.2, 3.3);
  Vector v2(1.5, 2.5, 3.5);

  Vector sum = v1 + v2;
  CHECK(sum == Vector(2.6, 4.7, 6.8));

  Vector diff = v1 - v2;
  CHECK(diff.equals(Vector(-0.4, -0.3, -0.2), 0.000000001));
}

TEST(Vector, MulDiv) {
  Vector v(1.2, 2.4, 3.6);
  Vector product = v * 2;
  CHECK(product == Vector(2.4, 4.8, 7.2));
  Vector quotient = v / 2;
  CHECK(quotient == Vector(0.6, 1.2, 1.8));
}

TEST(Vector, Length) {
  Vector v(2, 3, 4);
  DOUBLES_EQUAL(5.385165, v.length(), 0.000001);
}

TEST(Vector, LengthPow2) {
  Vector v(4, 5, 6);
  DOUBLES_EQUAL(77, v.length_pow2(), 0.0);
}

TEST(Vector, EqualsOperator) {
  Vector v1(4, 5, 6);
  Vector v2(4, 5, 6.1);
  Vector v3(400, 500, 600);
  CHECK(!(v1 == v2));
  v3 = v2;
  CHECK(v3 == v2);
}

TEST(Vector, EqualsWithTolerance) {
  Vector v1(4, 5, 6);
  Vector v2(4, 5, 6.1);
  CHECK_FALSE(v1.equals(v2, 0.099));
  CHECK(v1.equals(v2, 0.101));
}

TEST(Vector, Clear) {
  Vector v1(4, 5, 6);
  v1.clear();
  CHECK(v1 == Vector(0.0, 0.0, 0.0));
}
