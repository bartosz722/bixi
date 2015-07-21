#ifndef SPACECRAFT_H_
#define SPACECRAFT_H_

#include "PhysicalObject.h"

class Spacecraft: public PhysicalObject {
public:
  Spacecraft();
  virtual ~Spacecraft();

  virtual void moveToNextState();

  // Next state variables:
  double _deltaMass;
};

#endif /* SPACECRAFT_H_ */
