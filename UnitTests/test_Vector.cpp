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
  CHECK(vectorsEqual(sum, Vector(2.6, 4.7, 6.8)));

  Vector diff = v1 - v2;
  CHECK(vectorsEqual(diff, Vector(-0.4, -0.3, -0.2)));
}

TEST(Vector, MulDiv) {
  Vector v(1.2, 2.4, 3.6);
  Vector product = v * 2;
  CHECK(vectorsEqual(product, Vector(2.4, 4.8, 7.2)));
  Vector quotient = v / 2;
  CHECK(vectorsEqual(quotient, Vector(0.6, 1.2, 1.8)));
}

TEST(Vector, Length) {
  Vector v(2, 3, 4);
  DOUBLES_EQUAL(5.385165, v.length(), 0.000001);
}

TEST(Vector, LengthPow2) {
  Vector v(4, 5, 6);
  DOUBLES_EQUAL(77, v.length_pow2(), 0.0);
}

TEST(Vector, Equals) {
  Vector v1(4, 5, 6);
  Vector v2(4, 5, 6.1);
  CHECK(!(v1 == v2));
  v1 = v2;
  CHECK(v1 == v2);
}
