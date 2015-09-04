#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "SphericalObject.h"

TEST_GROUP(SphericalObject) {
};

TEST(SphericalObject, Init) {
  SphericalObject so;
  CHECK(PhysicalObjectType::SphericalObject == so.getType());
  CHECK_EQUAL(PhysicalObject::_invalidId, so.getId());
  CHECK(so._active);
}

//TODO: uzupełnić unit testy

