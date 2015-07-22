#include "SphericalObject.h"

SphericalObject::SphericalObject()
  : _radius(0.0)
{
  _type = PhysicalObjectType::SphericalObject;
}

SphericalObject::~SphericalObject() {
}

