#include "PhysicalObject.h"
#include "PhysicalDefinitions.h"
#include "Assert.h"

int PhysicalObject::_nextId = 0;

PhysicalObject::PhysicalObject()
  : _mass(0.0)
  , _type(PhysicalObjectType::PhysicalObject)
{
  _id = _nextId++;
}

PhysicalObject::~PhysicalObject() {
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

void PhysicalObject::moveToNextState() {
  // TODO
}

void PhysicalObject::clearNextStateVariables() {
  _force.clear();
}
