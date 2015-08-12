#include <iostream>
#include "Universe.h"
#include "Assert.h"

// temporary includes:
#include <cmath>
#include <cstdlib>
#include "SphericalObject.h"

using namespace std;

Universe::Snapshot::Snapshot()
  : _running(false), _currentTick(0), _elapsedTime(0.0), _collisionDetected(false)
{}

Universe::Universe() {
  resetSettings();
  resetRuntimeData();
}

Universe::~Universe() {
}

void Universe::resetSettings() {
  _timeUnit = 1.0;
  _detectCollision = false;
  _collisionTolerance = 0.00001;
}

void Universe::resetRuntimeData() {
  _running = false;
  _stopRequested = false;
  _currentTick = 0;
  _elapsedTime = 0.0;
  _collisionDetected = false;
}

bool Universe::loadSettings() {
  _timeUnit = 1.0;
  _detectCollision = true;
  _collisionTolerance = 0.00001;
  return true;
}

bool Universe::loadPhysicalObjects() {
  SphericalObject earth;
  earth.setId(1);
  earth._mass = 5.972 * pow(10, 24);
  earth._position = Vector(0, 0, 0);
  earth._velocity = Vector(0, 0, 0);
  earth._radius = 6370*1000;

  PhysicalObject man1;
  man1.setId(2);
  man1._mass = 70;
  man1._position = Vector(earth._radius + 10, 0, 0);
  man1._velocity = Vector(0, 0, 0);

  PhysicalObject iss;
  iss.setId(3);
  iss._mass = 417289;
  iss._position = Vector(earth._radius + 430*1000, 0, 0);
  iss._velocity = Vector(0, 7706, 0);

  SphericalObject someRock;
  someRock.setId(4);
  someRock._mass = earth._mass;
  someRock._position = Vector(earth._radius * 3, 0, 0);
  someRock._velocity = Vector(0, 3000, 0);
  someRock._radius = earth._radius;

  _objects.insert(earth);
//  _objects.insert(man1));
  _objects.insert(iss);
//  _objects.insert(someRock);


  for(auto const & po : _objects) {
    ASSERT(po->_mass != 0.0);
  }

  cout << "objects loaded, count: " << _objects.size() << endl;
  return true;
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
    {
      lock_guard<mutex> locker(_mutexData);
      if(_stopRequested) {
        break;
      }
      tick();
    }

    this_thread::sleep_for(chrono::milliseconds(1));
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
    po->moveToNextState(_timeUnit);
    po->clearNextStateVariables();
  }

  // Check for collisions
  if(_detectCollision) {
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
  _elapsedTime += _timeUnit;
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
  minDistance -= bigRadius * _collisionTolerance;

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
