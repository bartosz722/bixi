/*
 * Spacecraft.cpp
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#include "Spacecraft.h"
#include "Assert.h"

Spacecraft::Spacecraft()
  : _engineOn(false)
  , _propellantMass(0.0)
  , _thrustMassRate(0.0)
  , _thrustSpeed(0.0)
  , _propellantRunsOut(false)
  , _ejectedPropellantMass(0.0)
{
  _type = PhysicalObjectType::Spacecraft;
}

Spacecraft::~Spacecraft() {
}

std::unique_ptr<PhysicalObject> Spacecraft::copy() const {
  return std::unique_ptr<PhysicalObject>(new Spacecraft(*this));
}

void Spacecraft::copyFrom(const PhysicalObject & other) {
  ASSERT(getType() == other.getType());
  new (this) Spacecraft(static_cast<const Spacecraft &>(other));
}

void Spacecraft::calculateNextStateVariables(double deltaTime) {
  if(!_engineOn) {
    return;
  }

  Vector forceFromThruster;
  _ejectedPropellantMass = _thrustMassRate * deltaTime;

  if(_ejectedPropellantMass < _propellantMass) {
    _propellantRunsOut = false;
    forceFromThruster = _direction * (_thrustSpeed * _thrustMassRate);
  }
  else {
    _propellantRunsOut = true;
    _ejectedPropellantMass = _propellantMass;
    Vector minusEjectedMomentum = _direction * (_thrustSpeed * _ejectedPropellantMass);
    forceFromThruster = minusEjectedMomentum / deltaTime;
  }

  _force = _force + forceFromThruster;
}

void Spacecraft::moveToNextState(double deltaTime) {
  if(_engineOn) {
    _mass -= _ejectedPropellantMass;
    if(_propellantRunsOut) {
      _engineOn = false;
      _propellantMass = 0.0;
    }
    else {
      _propellantMass -= _ejectedPropellantMass;
    }
  }

  PhysicalObject::moveToNextState(deltaTime);
}

void Spacecraft::clearNextStateVariables() {
  PhysicalObject::clearNextStateVariables();
  _propellantRunsOut = false;
  _ejectedPropellantMass = 0.0;
}
