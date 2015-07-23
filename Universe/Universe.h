/*
 * Universe.h
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include "BasicDefinitions.h"
#include "PhysicalObjectsContainer.h"

class Universe {
public:
  Universe();
  ~Universe();

  bool start();

private:
  bool loadSettings();
  bool loadPhysicalObjects();
  void resetRuntimeData();
  void tick();
  bool objectsCollided(const PhysicalObject & obj1, const PhysicalObject & obj2);

  void printObjects();
  void printAfterTick();

  // settings:
  double _timeUnit; // s; tick time unit
  bool _detectCollision;
  double _collisionTolerance; // factor (0--1.0]

  // runtime data:
  TickT _currentTick; // always counted from 0
  double _elapsedTime; // s
  // DateTimeT _beginning
  bool _collisionDetected;

  // physical objects data:
  PhysicalObjectsContainer _objects;
};

#endif /* UNIVERSE_H_ */
