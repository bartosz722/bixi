#include <iostream>
#include "Universe.h"
#include "Assert.h"

#include "SphericalObject.h" // tmp

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
    if(_currentTick >= 3) {
      break;
    }
  }

  return true;
}

bool Universe::loadSettings() {
  _timeUnit = 1.0;
  return true;
}

bool Universe::loadPhysicalObjects() {
  PhysicalObject po;
  po._mass = 71.5;
  po._position = Vector(1000, 1000, 1000);

  PhysicalObject po2;
  po2._mass = 201;
  po2._position = Vector(2000, 3000, 4000);

  SphericalObject so;
  so._mass = 1000 * 1000;
  so._position = Vector(0, 0, 0);
  so._radius = 10;

  _objects.insert(unique_ptr<PhysicalObject>(new PhysicalObject(po)));
  _objects.insert(unique_ptr<PhysicalObject>(new PhysicalObject(po2)));
  _objects.insert(unique_ptr<PhysicalObject>(new SphericalObject(so)));

  cout << "objects loaded, count: " << _objects.size() << endl;
  return true;
}

void Universe::resetRuntimeData() {
  _currentTick = 0;
  _elapsedTime = 0.0;
}

// To jest częściowo nieaktualne:
// tick():
  // iteracja po obiektach
    // iteracja po obiektach
      // oblicz siłę
        // na podstawie stanów PhysicalObject
        // na podstawie właściwiości konkretnego obiektu (np. odrzut statku kosmicznego)
    // aktualizuj stan fizyczny
      // na podstawie obliczonej siły
      // z innych powodów (zmniejszanie masy podczas spalania paliwa)
    // aktuializuj właściwiości konkretnego obiektu

// inne elementy pętli (ale to moze byc po stronie GUI):
// - markery (np. okrąg wokół planety, odcinek między obiektami, odcinek między stałymi punktami)
// - liczniki (np. czas między zdarzeniami)

void Universe::tick() {
  cout << "tick " << _currentTick  << endl;

  // Calculate gravity force
  for(auto it1 = _objects.begin(); it1 != _objects.end(); ++it1) {
    PhysicalObject & obj1 = **it1;
    for(auto it2 = it1 + 1; it2 != _objects.end(); ++it2) {
      PhysicalObject & obj2 = **it2;
      Vector gForce = obj1.getGravityForce(obj2);
      obj1._force = obj1._force + gForce;
      obj2._force = obj2._force + gForce * -1.0;
      cout << "force between obj " << obj1.getId() << " and obj " << obj2.getId()
          << ": " << gForce << endl;
    }
  }

  for(auto & po : _objects) {
    cout << "force of object " << po->getId() << ": " << po->_force << endl;
    po->moveToNextState();
    po->clearNextStateVariables();
  }

  ++_currentTick;
  _elapsedTime += _timeUnit;
}
