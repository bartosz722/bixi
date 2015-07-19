#ifndef PHYSICALOBJECT_H_
#define PHYSICALOBJECT_H_

#include "Vector.h"
#include "PhysicalObjectType.h"

// Object with mass

class PhysicalObject {
public:
  PhysicalObject();
  virtual ~PhysicalObject();

  PhysicalObjectType getType() const { return _type; }
  virtual PhysicalObject * copy() const; // c++11 unique_ptr
  void copyFrom(const PhysicalObject & src);

  Vector getGravityForce(const PhysicalObject & other) const;

  // Should be used only for unit tests.
  bool equals(const PhysicalObject & other) const;

  // State:
  double _mass; // kg
  Vector _position; // m
  Vector _speed; // m/s, relative to [0,0,0]

protected:
  virtual void copyDataFrom(const PhysicalObject & src);
  virtual bool dataEquals(const PhysicalObject & other) const;

  PhysicalObjectType _type;

private:
  void operator=(PhysicalObject) {} // c++11 delete

};

#endif /* PHYSICALOBJECT_H_ */
