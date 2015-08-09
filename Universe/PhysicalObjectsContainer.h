#ifndef PHYSICALOBJECTSCONTAINER_H_
#define PHYSICALOBJECTSCONTAINER_H_

#include <vector>
#include <memory>

#include "BasicDefinitions.h"
#include "PhysicalObject.h"

class PhysicalObjectsContainer : public std::vector<std::unique_ptr<PhysicalObject>> {
public:
  PhysicalObjectsContainer() = default;
  PhysicalObjectsContainer(PhysicalObjectsContainer &) = delete;

  void operator=(const PhysicalObjectsContainer & other);

  void insert(std::unique_ptr<PhysicalObject> && po);
  void insert(const PhysicalObject & po);
};

#endif /* PHYSICALOBJECTSCONTAINER_H_ */
