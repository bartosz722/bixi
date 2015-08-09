#include <cmath>
#include "CppUTest/TestHarness.h"
#include "unit_tests_tools.h"
#include "PhysicalObjectsContainer.h"
#include "PhysicalObject.h"
#include "SphericalObject.h"

TEST_GROUP(PhysicalObjectsContainer) {
  bool basicCompare(const PhysicalObjectsContainer & poc1, const PhysicalObjectsContainer & poc2) {
    // TODO: full compare
    if(poc1.size() != poc2.size()) {
      return false;
    }

    PhysicalObjectsContainer::const_iterator it1 = poc1.begin();
    PhysicalObjectsContainer::const_iterator it2 = poc2.begin();
    while(it1 != poc1.end()) {
      if((*it1)->getId() != (*it2)->getId() ||
         (*it1)->getType() != (*it2)->getType()) {
        return false;
      }
      ++it1;
      ++it2;
    }

    return true;
  }
};

TEST(PhysicalObjectsContainer, InsertByPtr) {
  PhysicalObjectsContainer poc;
  for(int i=0; i<10; ++i) {
    poc.insert(std::unique_ptr<PhysicalObject>(new PhysicalObject));
    poc.insert(std::unique_ptr<PhysicalObject>(new SphericalObject));
  }
  CHECK_EQUAL(20, poc.size());
}

TEST(PhysicalObjectsContainer, InsertByRef) {
  PhysicalObjectsContainer poc;
  PhysicalObject po;
  SphericalObject so;
  int id = 1;
  for(int i=0; i<10; ++i) {
    po.setId(id++);
    so.setId(id++);
    poc.insert(po);
    poc.insert(so);
  }
  CHECK_EQUAL(20, poc.size());

  for(int i=0; i<20; ++i) {
    CHECK_EQUAL(i+1, poc[i]->getId());
  }
}

TEST(PhysicalObjectsContainer, OperatorAssignment) {
  PhysicalObjectsContainer poc;
  PhysicalObject po;
  SphericalObject so;
  int id = 1;
  for(int i=0; i<10; ++i) {
    po.setId(id++);
    so.setId(id++);
    poc.insert(po);
    poc.insert(so);
  }

  PhysicalObjectsContainer poc2;
  CHECK(!basicCompare(poc, poc2));
  poc2 = poc;
  CHECK(basicCompare(poc, poc2));

  // source bigger
  po.setId(id++);
  poc.insert(po);
  CHECK(!basicCompare(poc, poc2));
  poc2 = poc;
  CHECK(basicCompare(poc, poc2));

  // change type
  so.setId(id++);
  poc[0] = so.copy();
  CHECK(!basicCompare(poc, poc2));
  poc2 = poc;
  CHECK(basicCompare(poc, poc2));

  // source smaller
  poc.resize(poc.size()-1);
  CHECK(!basicCompare(poc, poc2));
  poc2 = poc;
  CHECK(basicCompare(poc, poc2));
}
