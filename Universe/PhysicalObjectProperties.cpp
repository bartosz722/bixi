#include <PhysicalObjectProperties.h>
#include "Assert.h"

PhysicalObjectProperties::PhysicalObjectProperties() {
  _id = PhysicalObject::_invalidId;
  _tracked = false;
}

void PhysicalObjectProperties::setId(int id) {
  ASSERT(_id == PhysicalObject::_invalidId);
  _id = id;
}

