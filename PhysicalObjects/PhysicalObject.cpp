#include "PhysicalObject.h"
#include "Assert.h"

PhysicalObject::PhysicalObject()
  : _mass(0.0)
  , _direction(1.0, 0, 0)
  , _active(true)
  , _type(PhysicalObjectType::PhysicalObject)
  , _id(_invalidId)
{
}

PhysicalObject::~PhysicalObject() {
}

std::unique_ptr<PhysicalObject> PhysicalObject::copy() const {
  return std::unique_ptr<PhysicalObject>(new PhysicalObject(*this));
}

void PhysicalObject::copyFrom(const PhysicalObject & other) {
  ASSERT(getType() == other.getType());
  new (this) PhysicalObject(other);
}

Vector PhysicalObject::getGravityForce(const PhysicalObject & other, double G) const {
  Vector distanceVect = distanceTo(other);
  double distance = distanceVect.length();
  double distance_pow2 = distanceVect.length_pow2();

  if(distance_pow2 == 0.0) {
    return Vector();
  }

  double force = G * _mass * other._mass / distance_pow2;
  // vF = F * -(vR / length(vR))
  Vector forceVect = distanceVect * (force * -1.0 / distance);

  return forceVect;
}

Vector PhysicalObject::distanceTo(const PhysicalObject & other) const {
  return _position - other._position;
}

void PhysicalObject::moveToNextState(double deltaTime) {
  _velocity = _velocity + _force * (deltaTime / _mass);
  _position = _position + _velocity * deltaTime;
}

void PhysicalObject::clearNextStateVariables() {
  _force.clear();
}

void PhysicalObject::setId(int id) {
  ASSERT(_id == _invalidId);
  _id = id;
}
