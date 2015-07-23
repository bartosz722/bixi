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

bool Universe::start() {
  cout << "Universe::start()\n";

  if(!loadSettings()) {
    return false;
  }

  if(!loadPhysicalObjects()) {
    return false;
  }

  resetRuntimeData();

  while(true) {
    tick();
    printAfterTick();
  }

  return true;
}

bool Universe::loadSettings() {
  _timeUnit = 1.0;
  return true;
}

bool Universe::loadPhysicalObjects() {
  PhysicalObject earth;
  earth._mass = 5.972 * pow(10, 24);
  earth._position = Vector(0, 0, 0);

  PhysicalObject man1;
  man1._mass = 1000000;//70;
  man1._position = Vector(6370*1000, 0, 0);

  PhysicalObject iss;
  iss._mass = 417289;
  iss._position = Vector((430 + 6370)*1000, 0, 0);
  iss._velocity = Vector(0, 7706, 0);

  _objects.insert(unique_ptr<PhysicalObject>(new PhysicalObject(earth)));
  _objects.insert(unique_ptr<PhysicalObject>(new PhysicalObject(iss)));

  for(auto const & po : _objects) {
    ASSERT(po->_mass != 0.0);
  }

  cout << "objects loaded, count: " << _objects.size() << endl;
  printObjects();
  return true;
}

void Universe::resetRuntimeData() {
  _currentTick = 0;
  _elapsedTime = 0.0;
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

  ++_currentTick;
  _elapsedTime += _timeUnit;
}
