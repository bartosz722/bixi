#ifndef PHYSICALOBJECT_H_
#define PHYSICALOBJECT_H_

#include "Vector.h"
#include "PhysicalObjectType.h"

// Object with mass

class PhysicalObject {
public:
  PhysicalObject();
  PhysicalObject(const PhysicalObject & other) = default;
  virtual ~PhysicalObject();

  PhysicalObjectType getType() const { return _type; }
  int getId() const { return _id; }
  void setId(int id) { _id = id; }

  // Attraction of this object by other object
  Vector getGravityForce(const PhysicalObject & other) const;
  // Distance vector pointed to this
  Vector distanceTo(const PhysicalObject & other) const;

  virtual void moveToNextState(double deltaTime);
  virtual void clearNextStateVariables();

  // State:
  double _mass; // kg
  Vector _position; // m
  Vector _velocity; // m/s, relative to [0,0,0]

  // Next state variables:
  Vector _force;

protected:
  PhysicalObjectType _type;

private:
  void operator=(PhysicalObject) = delete;

  int _id;
};

#endif /* PHYSICALOBJECT_H_ */
