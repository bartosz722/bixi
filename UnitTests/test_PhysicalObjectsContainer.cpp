#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "PhysicalObjectsContainer.h"
#include "PhysicalObject.h"
#include "SphericalObject.h"

TEST_GROUP(PhysicalObjectsContainer) {
};

TEST(PhysicalObjectsContainer, Insert) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.insert(std::unique_ptr<PhysicalObject>(new PhysicalObject));
    poc.insert(std::unique_ptr<PhysicalObject>(new SphericalObject));
  }
  CHECK_EQUAL(20, poc.size());
}
