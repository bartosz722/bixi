#include <utility>
#include "PhysicalObjectsContainer.h"

void PhysicalObjectsContainer::insert(std::unique_ptr<PhysicalObject> && po) {
  push_back(std::move(po));
}

void PhysicalObjectsContainer::insert(const PhysicalObject & po) {
  push_back(po.copy());
}

void PhysicalObjectsContainer::operator=(const PhysicalObjectsContainer & other) {
  if(size() != other.size()) {
    resize(other.size());
  }

  iterator it = begin();
  const_iterator otherIt = other.begin();
  while(it != end()) {
    if(it->get() != nullptr && (*it)->getType() == (*otherIt)->getType()) {
      (*it)->copyFrom(**otherIt); // no allocation when types are the same
    }
    else {
      *it = (*otherIt)->copy();
    }

    ++it;
    ++otherIt;
  }
}
