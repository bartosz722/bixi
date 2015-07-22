#include <utility>
#include "PhysicalObjectsContainer.h"

void PhysicalObjectsContainer::insert(std::unique_ptr<PhysicalObject> && po) {
  push_back(std::move(po));
}

