#include "CppUTest/TestHarness.h"
#include "Math.h"

TEST_GROUP(Math) {
};

TEST(Math, BallVolume) {
  double r = 10;
  double expectedVolume = 4188.790205;
  DOUBLES_EQUAL(expectedVolume, Math::ballVolume(r), 0.000001);
}

TEST(Math, BallRadius) {
  double V = 1000;
  double expectedRadius = 6.203504909;
  DOUBLES_EQUAL(expectedRadius, Math::ballRadius(V), 0.000000001);
}

