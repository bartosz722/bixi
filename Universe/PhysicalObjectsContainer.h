#ifndef PHYSICALOBJECTSCONTAINER_H_
#define PHYSICALOBJECTSCONTAINER_H_

#include <vector>

#include "BasicDefinitions.h"

class PhysicalObject;

class PhysicalObjectsContainer : public std::vector<PhysicalObject *> { // c++11 unique ptr
public:
  void append(PhysicalObject * po);
};

#endif /* PHYSICALOBJECTSCONTAINER_H_ */
