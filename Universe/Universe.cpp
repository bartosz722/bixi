#include <iostream>
#include "Universe.h"
#include "Assert.h"

#include "SphericalObject.h" // tmp

using namespace std;

Universe::Universe() {
  // TODO Auto-generated constructor stub

}

Universe::~Universe() {
  // TODO Auto-generated destructor stub
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
    if(_currentTick >= 10) {
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

  SphericalObject so;
  so._mass = 1000 * 1000;
  so._position = Vector(0, 0, 0);
  so._radius = 10;

  _objects.append(new PhysicalObject(po));
  _objects.append(new SphericalObject(so));

  return true;
}

void Universe::resetRuntimeData() {
  _currentTick = 0;
  _elapsedTime = 0.0;
}

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
  cout << "tick " << _currentTick
      << ", objects count: " << _objects.size()
      << endl;

  // TODO: poprawić to!

//  PhysicalObjectsContainer & objectsCurr = currentObjects();
//  PhysicalObjectsContainer & objectsNext = nextObjects();
//  ASSERT(objectsCurr.isCompatibleWith(objectsNext));
//
//  PhysicalObjectsContainer::IteratorT iterCurr = objectsCurr._objects.begin();
//  PhysicalObjectsContainer::IteratorT iterNext = objectsNext._objects.begin();
//
//  while(iterCurr != objectsCurr._objects.end()) {
//    Vector gravityForce;
//    Vector otherForce;
//
//    for(PhysicalObjectsContainer::IteratorT iterCurrOther = objectsCurr._objects.begin();
//        iterCurrOther != objectsCurr._objects.end();
//        ++iterCurrOther) {
//      if(iterCurr == iterCurrOther) {
//        continue;
//      }
//
////      Vector gf = (*iterCurr)->getGravityForce(**iterCurrOther);
////      cout << "gf: " << gf << endl;
//
//      gravityForce = gravityForce + (*iterCurr)->getGravityForce(**iterCurrOther);
////      otherForce = otherForce + computeOtherForce(**iterCurr, **iterCurrOther);
//    }
//
//    cout << "gravityForce: " << gravityForce << endl;
//
//    ++iterCurr;
//    ++iterNext;
//  }
//
//  _objectsAAreCurrent = !_objectsAAreCurrent;
//  ++_currentTick;
//  _elapsedTime += _timeUnit;
//
//  objectsNext._tick = _currentTick;
//  objectsNext._elapsedTime = _elapsedTime;
}
