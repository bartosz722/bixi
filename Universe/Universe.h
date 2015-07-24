/*
 * Universe.h
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <mutex>
#include <thread>
#include "BasicDefinitions.h"
#include "PhysicalObjectsContainer.h"

class Universe {
public:
  Universe();
  ~Universe();

  bool loadSettings();
  bool loadPhysicalObjects();
  bool start();
  void waitForFinish();

private:
  void spacetime(); // main loop is executed here
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

  // Thread executed after start() has finished.
  std::thread _thread;
  // Guards runtime data and physical objects after start() has finished.
  std::mutex _mutexData;
};

#endif /* UNIVERSE_H_ */
