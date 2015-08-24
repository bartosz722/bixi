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
#include <chrono>
#include "BasicDefinitions.h"
#include "PhysicalObjectsContainer.h"
#include "PhysicalObjectProperties.h"

// Marks:
// TS - method is thread-safe and may be called only after start() was called.
// AC - method is thread-safe and may always called

class Universe {
public:
  struct Settings {
    Settings();
    double _timeUnit; // s; tick time unit
    bool _detectCollision;
    double _collisionTolerance; // factor (0--1.0]
    std::size_t _roundsPerSecond; // set to 0 for no limitation
    std::size_t _ticksPerRound; // set to 0 for no limitation
  };

  struct Snapshot {
    Snapshot();
    bool _running;
    TickT _currentTick;
    double _elapsedTime;
    bool _collisionDetected;
    PhysicalObjectsContainer _objects;
  };

  typedef std::vector<PhysicalObjectProperties> PropertiesT;

  Universe();
  ~Universe();

  void setSettings(const Settings & s);
  void insertPhysicalObject(const PhysicalObject & po, const PhysicalObjectProperties & prop);
  const PropertiesT & getPhysicalObjectsProperties() const { return _properties; }
  bool start();
  void stop(); // TS; can not be started after is stopped

  void waitForFinish(); // TS
  void getSnapshot(Snapshot & s); // AC; may be called before start()

private:
  typedef std::chrono::steady_clock ClockT;
  typedef ClockT::duration DurationT;

  void resetSettings();
  void resetRuntimeData();

  void spacetime(); // main loop is executed here
  void setRuntimeDataToStartValues();
  void tick();
  bool objectsCollided(const PhysicalObject & obj1, const PhysicalObject & obj2);

  // settings:
  Settings _sett;
  DurationT _roundDuration; // if zero then no sleep between rounds

  // synchronized runtime data:
  bool _running;
  bool _stopRequested;
  TickT _currentTick; // always counted from 0
  double _elapsedTime; // s
  // DateTimeT _beginning
  bool _collisionDetected;

  // not synchronized runtime data:
  ClockT::time_point _roundBegin;

  // synchronized physical objects data:
  PhysicalObjectsContainer _objects;

  // not synchronized physical objects data:
  // read-only after call to start(), consistent with _objects
  std::vector<PhysicalObjectProperties> _properties;

  // Thread executed after start() has finished.
  std::thread _thread;
  // Guards synchronized runtime data and synchronized physical objects data
  // while _thread is running.
  std::mutex _mutexData;
  // Id that will be assigned to next inserted object.
  int _nextPhysicalObjectId;
};

#endif /* UNIVERSE_H_ */
