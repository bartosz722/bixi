/*
 * Spacecraft.cpp
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#include "Spacecraft.h"

Spacecraft::Spacecraft()
  : _deltaMass(0.0)
{
  _type = PhysicalObjectType_Spacecraft;
}

Spacecraft::~Spacecraft() {
  // TODO Auto-generated destructor stub
}

void Spacecraft::moveToNextState() {
  // TODO
}
