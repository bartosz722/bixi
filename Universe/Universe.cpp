#include <iostream>
#include "Universe.h"
#include "Assert.h"
#include "SphericalObject.h"

using namespace std;

Universe::Settings::Settings()
  : _timeUnit(1.0), _detectCollision(false), _collisionTolerance(0.00001)
  , _roundsPerSecond(50), _ticksPerRound(10)
{}

Universe::Snapshot::Snapshot()
  : _running(false), _currentTick(0), _elapsedTime(0.0), _collisionDetected(false)
{}

Universe::Universe() {
  resetSettings();
  resetRuntimeData();
  _nextPhysicalObjectId = 0;
}

Universe::~Universe() {
}

void Universe::resetSettings() {
  _sett = Settings();
  _roundDuration = DurationT::zero();
}

void Universe::resetRuntimeData() {
  _running = false;
  _stopRequested = false;
  _currentTick = 0;
  _elapsedTime = 0.0;
  _collisionDetected = false;

  _roundBegin = ClockT::now();
}

void Universe::setSettings(const Settings & s) {
  _sett = s;

  if(_sett._roundsPerSecond == 0) {
    _roundDuration = DurationT::zero();
  }
  else {
    _roundDuration = DurationT(chrono::seconds(1)) / _sett._roundsPerSecond;
  }

  cout << "_roundsPerSecond: " << _sett._roundsPerSecond << ", _roundDuration: "
      << chrono::duration_cast<chrono::duration<double, ratio<1, 1000>>>(_roundDuration).count()
      << " ms" << endl;

  if(_sett._ticksPerRound == 0) {
    _sett._ticksPerRound = 1000000; // some big value
  }
  cout << "_ticksPerRound: " << _sett._ticksPerRound << endl;
}

void Universe
::insertPhysicalObject(const PhysicalObject & po, const PhysicalObjectProperties & prop) {
  ASSERT(po._mass != 0.0);
  int id = _nextPhysicalObjectId++;
  _objects.insert(po);
  _objects.back()->setId(id);
  _properties.push_back(prop);
  _properties.back().setId(id);
}

bool Universe::start() {
  ASSERT(!_stopRequested);
  cout << "Universe::start()\n";
  setRuntimeDataToStartValues();
  _thread = thread(&Universe::spacetime, this);
  return true;
}

void Universe::stop() {
  lock_guard<mutex> locker(_mutexData);
  _stopRequested = true;
}

void Universe::waitForFinish() {
  _thread.join();
}

void Universe::spacetime() {
  while(true) {
    _roundBegin = ClockT::now();

    for(std::size_t t=0; t<_sett._ticksPerRound; ++t) {
      lock_guard<mutex> locker(_mutexData);
      if(_stopRequested) {
        break;
      }
      tick();
    }

    auto elapsedTime = ClockT::now() - _roundBegin;
    auto timeToSleep = _roundDuration - elapsedTime;
    if(timeToSleep > timeToSleep.zero()) {
      this_thread::sleep_for(timeToSleep);
    }
  }
}

void Universe::setRuntimeDataToStartValues() {
  _running = true;
  _stopRequested = false;
  _currentTick = 0;
  _elapsedTime = 0.0;
  _collisionDetected = false;
}

void Universe::tick() {
  _collisionDetected = false;

  // Calculate gravity force
  for(auto it1 = _objects.cbegin(); it1 != _objects.cend(); ++it1) {
    PhysicalObject & obj1 = **it1;
    for(auto it2 = it1 + 1; it2 != _objects.cend(); ++it2) {
      PhysicalObject & obj2 = **it2;
      Vector gForce = obj1.getGravityForce(obj2);
      obj1._force = obj1._force + gForce;
      obj2._force = obj2._force + gForce * -1.0;
    }
  }

  for(auto const & po : _objects) {
    po->moveToNextState(_sett._timeUnit);
    po->clearNextStateVariables();
  }

  // Check for collisions
  if(_sett._detectCollision) {
    for(auto it1 = _objects.cbegin(); it1 != _objects.cend(); ++it1) {
      const PhysicalObject & obj1 = **it1;
      for(auto it2 = it1 + 1; it2 != _objects.cend(); ++it2) {
        const PhysicalObject & obj2 = **it2;
        if(objectsCollided(obj1, obj2)) {
          _collisionDetected = true;
        }
      }
    }
  }

  ++_currentTick;
  _elapsedTime += _sett._timeUnit;
}

bool Universe::objectsCollided(const PhysicalObject & obj1, const PhysicalObject & obj2) {
  const PhysicalObject * objects[] = { &obj1, &obj2 };
  double minDistance = 0.0;
  double bigRadius = 0.0;
  for(int i=0; i<2; ++i) {
    if(objects[i]->getType() == PhysicalObjectType::SphericalObject) {
      const SphericalObject & so= static_cast<const SphericalObject &>(*objects[i]);
      minDistance += so._radius;
      if(so._radius > bigRadius) {
        bigRadius = so._radius;
      }
    }
  }
  minDistance -= bigRadius * _sett._collisionTolerance;

  // We'll detect collision only when one object has non-zero radius.
  const double distance = obj1.distanceTo(obj2).length();
  bool collision = distance < minDistance;
  return collision;
}

void Universe::getSnapshot(Snapshot & s) {
  lock_guard<mutex> locker(_mutexData);
  s._running = _running;
  s._currentTick = _currentTick;
  s._elapsedTime = _elapsedTime;
  s._collisionDetected = _collisionDetected;
  s._objects = _objects;
}
