#ifndef SPHERICALOBJECT_H_
#define SPHERICALOBJECT_H_

#include "PhysicalObject.h"

class SphericalObject: public PhysicalObject {
public:
  SphericalObject();
  virtual ~SphericalObject();

  virtual std::unique_ptr<PhysicalObject> copy() const;
  virtual void copyFrom(const PhysicalObject & other);

  double _radius; // m

protected:
  SphericalObject(const SphericalObject & other) = default;

private:

};

#endif /* SPHERICALOBJECT_H_ */
