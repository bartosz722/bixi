#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "PhysicalObjectsContainer.h"
#include "PhysicalObject.h"
#include "SphericalObject.h"

TEST_GROUP(PhysicalObjectsContainer) {
  void setup() {
    c = 0;
  }

  PhysicalObject * getSomePhysicalObject() {
    ++c;
    PhysicalObject * o = new PhysicalObject;;
    o->_mass = 1.11 * c;
    o->_position = Vector(5.5 * c, 6.6 * c, 7.7 * c);
    o->_speed =  Vector(-2.21 * c, -3.6 * c, 5.01 * c);
    return o;
  }

  SphericalObject * getSomeSphericalObject() {
    ++c;
    SphericalObject * so = new SphericalObject;
    so->_mass = 1.11 * c;
    so->_position = Vector(5.5 * c, 6.6 * c, 7.7 * c);
    so->_speed =  Vector(-2.21 * c, -3.6 * c, 5.01 * c);
    so->_radius = 71.3 * c;
    return so;
  }

  int c;
};

TEST(PhysicalObjectsContainer, DeafultConstructedContainer) {
  PhysicalObjectsContainer poc;
  CHECK_EQUAL(0, poc.count());
  CHECK_EQUAL(0, poc._tick);
  DOUBLES_EQUAL(0.0, poc._elapsedTime, 0.0);
}

TEST(PhysicalObjectsContainer, AppendCount) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.append(getSomePhysicalObject());
    poc.append(getSomeSphericalObject());
  }
  CHECK_EQUAL(20, poc.count());
}

TEST(PhysicalObjectsContainer, Clear) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.append(getSomePhysicalObject());
    poc.append(getSomeSphericalObject());
  }
  poc.clear();
  CHECK_EQUAL(0, poc.count());
  CHECK_EQUAL(0, poc._tick);
  DOUBLES_EQUAL(0.0, poc._elapsedTime, 0.0);
}

TEST(PhysicalObjectsContainer, OperatorAssignment) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.append(getSomePhysicalObject());
    poc.append(getSomeSphericalObject());
  }
  PhysicalObjectsContainer poc2;
  poc2 = poc;

  CHECK(poc._tick == poc2._tick);
  CHECK(poc._elapsedTime == poc2._elapsedTime);
  CHECK_EQUAL(poc.count(), poc2.count());

  PhysicalObjectsContainer::IteratorT it1 = poc._objects.begin();
  PhysicalObjectsContainer::IteratorT it2 = poc2._objects.begin();
  while(it1 != poc._objects.end()) {
    CHECK((*it1)->equals(**it2));
    ++it1;
    ++it2;
  }
}

TEST(PhysicalObjectsContainer, IsCompatibleWith) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.append(getSomePhysicalObject());
    poc.append(getSomeSphericalObject());
  }
  PhysicalObjectsContainer poc2;
  CHECK(!poc.isCompatibleWith(poc2));

  poc2 = poc;
  CHECK(poc.isCompatibleWith(poc2));

  poc.append(getSomePhysicalObject());
  CHECK(!poc.isCompatibleWith(poc2));
  poc2.append(getSomeSphericalObject());
  CHECK(!poc.isCompatibleWith(poc2));
}
