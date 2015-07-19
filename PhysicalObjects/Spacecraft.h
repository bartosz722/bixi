/*
 * Spacecraft.h
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#ifndef SPACECRAFT_H_
#define SPACECRAFT_H_

#include "PhysicalObject.h"

class Spacecraft: public PhysicalObject {
public:
  Spacecraft();
  virtual ~Spacecraft();

  virtual PhysicalObject * copy() const { return 0; }
};

#endif /* SPACECRAFT_H_ */
