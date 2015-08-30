#ifndef UNIVERSE_COLLISIONCOMPARATOR_H_
#define UNIVERSE_COLLISIONCOMPARATOR_H_

#include "PhysicalObject.h"
#include "Assert.h"

// "Small" object first
struct CollisionComparator {
  bool operator()(PhysicalObject * o1, PhysicalObject * o2) {
    if(o1->_mass == o2->_mass) {
      ASSERT(o1->getId() != o2->getId());
      return o1->getId() < o2->getId();
    }

    return o1->_mass < o2->_mass;
  }
};


#endif /* UNIVERSE_COLLISIONCOMPARATOR_H_ */
