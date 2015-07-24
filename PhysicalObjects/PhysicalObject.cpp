#include "PhysicalObject.h"
#include "PhysicalDefinitions.h"
#include "Assert.h"

PhysicalObject::PhysicalObject()
  : _mass(0.0)
  , _type(PhysicalObjectType::PhysicalObject)
  , _id(0)
{
}

PhysicalObject::~PhysicalObject() {
}

Vector PhysicalObject::getGravityForce(const PhysicalObject & other) const {
  Vector distanceVect = distanceTo(other);
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

Vector PhysicalObject::distanceTo(const PhysicalObject & other) const {
  return _position - other._position;
}

void PhysicalObject::moveToNextState(double deltaTime) {
  Vector acc = _force / _mass;
  Vector acc_t = acc * deltaTime;

  // s = v0*t + 0.5 * a*t^2 = t(v0 + 0.5 * a*t)
  Vector deltaPosition = (_velocity + acc_t / 2) * deltaTime;
  _position = _position + deltaPosition;
  // v = v0 + a*t
  _velocity = _velocity + acc_t;
}

void PhysicalObject::clearNextStateVariables() {
  _force.clear();
}
