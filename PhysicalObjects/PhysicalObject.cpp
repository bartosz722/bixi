#include "PhysicalObject.h"
#include "PhysicalDefinitions.h"
#include "Assert.h"

PhysicalObject::PhysicalObject()
  : _mass(0.0)
  , _type(PhysicalObjectType_PhysicalObject)
{
}

PhysicalObject::~PhysicalObject() {
}

PhysicalObject * PhysicalObject::copy() const {
  return new PhysicalObject(*this);
}

void PhysicalObject::copyFrom(const PhysicalObject & src) {
  ASSERT(_type == src._type);
  copyDataFrom(src);
}

void PhysicalObject::copyDataFrom(const PhysicalObject & src) {
  _mass = src._mass;
  _position = src._position;
  _speed = src._speed;
}

bool PhysicalObject::equals(const PhysicalObject & other) const {
  if(_type != other._type) {
    return false;
  }
  return dataEquals(other);
}

bool PhysicalObject::dataEquals(const PhysicalObject & other) const {
  return _mass == other._mass &&
         _position == other._position &&
         _speed == other._speed;
}

Vector PhysicalObject::getGravityForce(const PhysicalObject & other) const {
  Vector distanceVect = _position - other._position;
  double distance = distanceVect.length();
  double distance_pow2 = distanceVect.length_pow2();

  if(distance_pow2 == 0.0) {
    return Vector();
  }

  double force = constG * _mass * other._mass / distance_pow2;
  // vF = F * -(vR / length(vR))
  Vector forceVect = distanceVect * (force * -1.0 / distance);

  return forceVect;
}
