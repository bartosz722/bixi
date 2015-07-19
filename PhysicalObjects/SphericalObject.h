#ifndef SPHERICALOBJECT_H_
#define SPHERICALOBJECT_H_

#include "PhysicalObject.h"

class SphericalObject: public PhysicalObject {
public:
  SphericalObject();
  virtual ~SphericalObject();

  virtual PhysicalObject * copy() const; // c++11 unique_ptr

  double _radius; // m

protected:
  virtual void copyDataFrom(const PhysicalObject & src);
  virtual bool dataEquals(const PhysicalObject & other) const;

private:

};

#endif /* SPHERICALOBJECT_H_ */
