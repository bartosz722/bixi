#include "SphericalObject.h"
#include "Assert.h"

SphericalObject::SphericalObject()
  : _radius(0.0)
{
  _type = PhysicalObjectType::SphericalObject;
}

SphericalObject::~SphericalObject() {
}

std::unique_ptr<PhysicalObject> SphericalObject::copy() const {
  return std::unique_ptr<PhysicalObject>(new SphericalObject(*this));
}

void SphericalObject::copyFrom(const PhysicalObject & other) {
  ASSERT(getType() == other.getType());
  new (this) SphericalObject(static_cast<const SphericalObject &>(other));
}
