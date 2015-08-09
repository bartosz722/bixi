#include <iostream>
#include "Universe.h"
#include "Assert.h"

// temporary includes:
#include <cmath>
#include <cstdlib>
#include "SphericalObject.h"

using namespace std;

Universe::Universe() {
}

Universe::~Universe() {
}

bool Universe::loadSettings() {
  _timeUnit = 1.0;
  _detectCollision = true;
  _collisionTolerance = 0.00001;
  return true;
}

bool Universe::loadPhysicalObjects() {
  SphericalObject earth;
  earth._mass = 5.972 * pow(10, 24);
  earth._position = Vector(0, 0, 0);
  earth._radius = 6370*1000;

  PhysicalObject man1;
  man1._mass = 70;
  man1._position = Vector(earth._radius + 10, 0, 0);
  man1._velocity = Vector(0, 0, 0);

  PhysicalObject iss;
  iss._mass = 417289;
  iss._position = Vector(earth._radius + 430*1000, 0, 0);
  iss._velocity = Vector(0, 7706, 0);

  _objects.insert(earth);
//  _objects.insert(man1));
  _objects.insert(iss);

  for(auto const & po : _objects) {
    ASSERT(po->_mass != 0.0);
  }

  cout << "objects loaded, count: " << _objects.size() << endl;
  printObjects();
  return true;
}

bool Universe::start() {
  cout << "Universe::start()\n";
  resetRuntimeData();
  _thread = thread(&Universe::spacetime, this);
  return true;
}

void Universe::waitForFinish() {
  _thread.join();
}

void Universe::spacetime() {
  cout << "Universe::spacetime()\n";
  while(true) {
    lock_guard<mutex> locker(_mutexData);
    tick();
    printAfterTick();
    if(_collisionDetected) {
      printObjects();
      break; // We don't know what to do
    }
  }
}

void Universe::resetRuntimeData() {
  _currentTick = 0;
  _elapsedTime = 0.0;
  _collisionDetected = false;
}

void Universe::printObjects() {
  for(auto const & po : _objects) {
    cout << "object " << po->getId()
        << ": position " << po->_position
        << ", velocity " << po->_velocity << " (abs " << po->_velocity.length() << ")"
        << endl;
  }
}

void Universe::printAfterTick() {
  static const TickT freq = 100;
  static const int maxPrintCount = 50;

  static int printCount = 0;
  if(_currentTick % freq == 0) {
    cout << "tick " << _currentTick  << endl;
    printObjects();
    if(printCount++ >= maxPrintCount) {
      exit(0);
    }
  }
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

  if(collision) {
    cout << "After tick " << _currentTick
        << ": collision between object " << obj1.getId() << " and " << obj2.getId()
        << ", distance " << distance << ", minDistance " << minDistance
        << endl;
  }

  return collision;
}
