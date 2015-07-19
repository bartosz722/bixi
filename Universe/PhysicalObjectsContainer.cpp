#include "PhysicalObjectsContainer.h"
#include "PhysicalObject.h"

PhysicalObjectsContainer::PhysicalObjectsContainer() {
  clear();
}

PhysicalObjectsContainer::~PhysicalObjectsContainer() {
  clear();
}

void PhysicalObjectsContainer::clear() {
  _tick = 0;
  _elapsedTime = 0.0;
  for(IteratorT it = _objects.begin(); it != _objects.end(); ++it) {
    delete *it;
  }
  _objects.clear();
}

int PhysicalObjectsContainer::count() const {
  return _objects.size();
}

void PhysicalObjectsContainer::append(PhysicalObject * po) {
  _objects.push_back(po);
}

void PhysicalObjectsContainer::operator=(const PhysicalObjectsContainer & other) {
  clear();
  _tick = other._tick;
  _elapsedTime = other._elapsedTime;
  for(IteratorT it = other._objects.begin(); it != other._objects.end(); ++it) {
    _objects.push_back((*it)->copy());
  }
}

bool PhysicalObjectsContainer::isCompatibleWith(const PhysicalObjectsContainer & other) const {
  if(count() != other.count()) {
    return false;
  }

  IteratorT it1 = _objects.begin();
  IteratorT it2 = other._objects.begin();

  while(it1 != _objects.end()) {
    if((*it1)->getType() != (*it2)->getType()) {
      return false;
    }
    ++it1;
    ++it2;
  }

  return true;
}
