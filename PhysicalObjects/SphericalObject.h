#ifndef SPHERICALOBJECT_H_
#define SPHERICALOBJECT_H_

#include "PhysicalObject.h"

class SphericalObject: public PhysicalObject {
public:
  SphericalObject();
  virtual ~SphericalObject();

  double _radius; // m

protected:

private:

};

#endif /* SPHERICALOBJECT_H_ */
