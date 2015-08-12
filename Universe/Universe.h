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
  struct Snapshot {
    Snapshot();
    bool _running;
    TickT _currentTick;
    double _elapsedTime;
    bool _collisionDetected;
    PhysicalObjectsContainer _objects;
  };

  Universe();
  ~Universe();

  bool loadSettings();
  bool loadPhysicalObjects();
  bool start();
  void stop(); // can not be started after is stopped

  void waitForFinish();
  void getSnapshot(Snapshot & s); // may be always called

private:
  void resetSettings();
  void resetRuntimeData();

  void spacetime(); // main loop is executed here
  void setRuntimeDataToStartValues();
  void tick();
  bool objectsCollided(const PhysicalObject & obj1, const PhysicalObject & obj2);

  // settings:
  double _timeUnit; // s; tick time unit
  bool _detectCollision;
  double _collisionTolerance; // factor (0--1.0]

  // runtime data:
  bool _running;
  bool _stopRequested;
  TickT _currentTick; // always counted from 0
  double _elapsedTime; // s
  // DateTimeT _beginning
  bool _collisionDetected;

  // physical objects data:
  PhysicalObjectsContainer _objects;

  // Thread executed after start() has finished.
  std::thread _thread;
  // Guards runtime and physical objects data while _thread is running.
  std::mutex _mutexData;
};

#endif /* UNIVERSE_H_ */
