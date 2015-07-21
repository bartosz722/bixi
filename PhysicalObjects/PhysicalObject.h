#ifndef PHYSICALOBJECT_H_
#define PHYSICALOBJECT_H_

#include "Vector.h"
#include "PhysicalObjectType.h"

// Object with mass

class PhysicalObject {
public:
  PhysicalObject();
  virtual ~PhysicalObject();
  //PhysicalObject(PhysicalObject&) = default; // c++11

  PhysicalObjectType getType() const { return _type; }
  Vector getGravityForce(const PhysicalObject & other) const;

  // Moves object to next state and clears next state variables
  virtual void moveToNextState();

  // State:
  double _mass; // kg
  Vector _position; // m
  Vector _speed; // m/s, relative to [0,0,0]

  // Next state variables:
  Vector _force;

protected:
  PhysicalObjectType _type;

private:
  void operator=(PhysicalObject) {} // c++11 delete

};

#endif /* PHYSICALOBJECT_H_ */
