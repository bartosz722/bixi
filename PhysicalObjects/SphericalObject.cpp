#include "SphericalObject.h"

SphericalObject::SphericalObject()
  : _radius(0.0)
{
  _type = PhysicalObjectType_SphericalObject;
}

SphericalObject::~SphericalObject() {
  // TODO Auto-generated destructor stub
}

PhysicalObject *SphericalObject::copy() const {
  return new SphericalObject(*this);
}

void SphericalObject::copyDataFrom(const PhysicalObject & src) {
  const SphericalObject & so = static_cast<const SphericalObject &>(src);
  PhysicalObject::copyDataFrom(so);
  _radius = so._radius;
}

bool SphericalObject::dataEquals(const PhysicalObject & other) const {
  const SphericalObject & so = static_cast<const SphericalObject &>(other);
  return PhysicalObject::dataEquals(so) &&
         _radius == so._radius;
}

