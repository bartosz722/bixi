#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include "Universe.h"
#include "Assert.h"
#include "SphericalObject.h"
#include "Spacecraft.h"
#include "CollisionComparator.h"
#include "Math.h"

using namespace std;

Universe::Settings::Settings()
  : _timeUnit(1.0), _G(1)
  , _collision(CollisionBehaviour::None), _collisionTolerance(0.00001)
  , _roundsPerSecond(50), _ticksPerRound(10)
{}

Universe::Snapshot::Snapshot()
  : _running(false), _currentTick(0), _elapsedTime(0.0), _stoppedByCollision(false)
{}

Universe::Universe()
  : _precisionTester(_objects)
{
  resetSettings();
  resetRuntimeData();
  _nextPhysicalObjectId = 0;
}

Universe::~Universe() {
}

void Universe::resetSettings() {
  _sett = Settings();
  _roundDuration = DurationT::zero();
  _precisionTestMode = false;
}

void Universe::resetRuntimeData() {
  _running = false;
  _stopRequested = false;
  _currentTick = 0;
  _elapsedTime = 0.0;
  _stoppedByCollision = false;

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
    _sett._ticksPerRound = numeric_limits<size_t>::max();
  }
  cout << "_ticksPerRound: " << _sett._ticksPerRound << endl;
}

int Universe
::insertPhysicalObject(const PhysicalObject & po, const PhysicalObjectProperties & prop) {
  ASSERT(po._mass != 0.0);
  const int id = _nextPhysicalObjectId++;
  _objects.insert(po);
  _objects.back()->setId(id);
  _properties.push_back(prop);
  _properties.back().setId(id);
  return id;
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

    for(size_t t=0; t<_sett._ticksPerRound; ++t) {
      lock_guard<mutex> locker(_mutexData);
      if(_stopRequested || _stoppedByCollision) {
        _running = false;
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
  _stoppedByCollision = false;
}

void Universe::tick() {
  for(auto const & po : _objects) {
    if(!po->_active) {
      continue;
    }
    po->clearNextStateVariables();
  }

  // Let the pilot control his spacecraft
  if(_pilot) {
    _pilot->executeCurrentActions(_elapsedTime);
  }

  // Calculate next state of objects
  for(auto it1 = _objects.cbegin(); it1 != _objects.cend(); ++it1) {
    PhysicalObject & obj1 = **it1;
    if(!obj1._active) {
      continue;
    }

    // Gravity force
    for(auto it2 = it1 + 1; it2 != _objects.cend(); ++it2) {
      PhysicalObject & obj2 = **it2;
      if(!obj2._active) {
        continue;
      }

      Vector gForce = obj1.getGravityForce(obj2, _sett._G);
      obj1._force = obj1._force + gForce;
      obj2._force = obj2._force + gForce * -1.0;
    }

    // Thrust of spacecraft engine
    if(obj1.getType() == PhysicalObjectType::Spacecraft) {
      Spacecraft & s = static_cast<Spacecraft &>(obj1);
      s.calculateNextStateVariables(_sett._timeUnit);
    }
  }

  // Move objects to next state
  for(auto const & po : _objects) {
    if(!po->_active) {
      continue;
    }
    po->moveToNextState(_sett._timeUnit);
  }

  // From now on objects can be deactivated.

  // Check for collisions
  if(_sett._collision != CollisionBehaviour::None) {
    checkForCollisions();
  }

  ++_currentTick;
  _elapsedTime += _sett._timeUnit;

  if(_precisionTestMode) {
    if(!_precisionTester.precisionTestTick()) {
      _stopRequested = true;
    }
  }
}

void Universe::checkForCollisions() {
  for(auto it1 = _objects.cbegin(); it1 != _objects.cend(); ++it1) {
    PhysicalObject & obj1 = **it1;
    if(!obj1._active) {
      continue;
    }

    for(auto it2 = it1 + 1; it2 != _objects.cend(); ++it2) {
      PhysicalObject & obj2 = **it2;
      if(!obj2._active) {
        continue;
      }

      if(objectsCollided(obj1, obj2)) {
        if(_sett._collision == CollisionBehaviour::StopUniverse) {
          _stoppedByCollision = true;
        }
        else if(_sett._collision == CollisionBehaviour::Inelastic) {
          inelasticCollision(obj1, obj2);
        }
        else {
          ASSERT(false);
        }
      }
    }
  }
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

void Universe::inelasticCollision(PhysicalObject & obj1, PhysicalObject & obj2) {
  vector<PhysicalObject *> objects = { &obj1, &obj2 };
  sort(objects.begin(), objects.end(), CollisionComparator());
  PhysicalObject & small = *objects[0];
  PhysicalObject & big = *objects[1];

  big._velocity =
      (big._velocity * big._mass + small._velocity * small._mass)
      / (big._mass + small._mass);
  big._mass = big._mass + small._mass;
  small._active = false;

  cout << "inelastic collision: big id " << big.getId() << ", small id " << small.getId() << endl;

  if(big.getType() == PhysicalObjectType::SphericalObject &&
     small.getType() == PhysicalObjectType::SphericalObject) {
    SphericalObject & big_s = static_cast<SphericalObject &>(big);
    SphericalObject & small_s = static_cast<SphericalObject &>(small);
    double bigRadiusPre = big_s._radius;
    double totalVolume = Math::ballVolume(big_s._radius) + Math::ballVolume(small_s._radius);
    big_s._radius = Math::ballRadius(totalVolume);
    cout << "inelastic collision: radius big " << bigRadiusPre
        << ", radius small " << small_s._radius
        << ", radius big after " << big_s._radius
        << endl;
  }
}

void Universe::getSnapshot(Snapshot & s) {
  lock_guard<mutex> locker(_mutexData);
  s._running = _running;
  s._currentTick = _currentTick;
  s._elapsedTime = _elapsedTime;
  s._stoppedByCollision = _stoppedByCollision;
  s._objects = _objects;
}

void Universe::setUpPrecisionTest(const PrecisionTester::Settings & ps) {
  _precisionTestMode = true;
  _precisionTester.setUp(ps);

  Settings s;
  s._timeUnit = ps._timeUnit;
  s._G = ps._G;
  s._collision = CollisionBehaviour::StopUniverse;
  s._collisionTolerance = 0.00001;
  s._roundsPerSecond = ps._roundsPerSecond;
  s._ticksPerRound = ps._ticksPerRound;
  setSettings(s);

  PrecisionTester::TestObjects to = _precisionTester.getTestObjects();
  PhysicalObjectProperties pop;
  pop._tracked = true;
  for(const auto & o : to) {
    insertPhysicalObject(*o, pop);
  }
}

PrecisionTester::Result Universe::getPrecisionTestResult() {
  lock_guard<mutex> locker(_mutexData);
  return _precisionTester.getPrecisionTestResult();
}

void Universe::addActionForPilot(int spacecraftId, Pilot::ActionData ad) {
  for(const auto & po : _objects) {
    if(po->getId() == spacecraftId && po->getType() == PhysicalObjectType::Spacecraft) {
      Spacecraft & sc = static_cast<Spacecraft &>(*po);
      if(_pilot) {
        if(_pilot->getSpacecraftId() != sc.getId()) {
          cout << "Pilot already created for other spacecraft\n";
          return;
        }
      }
      else {
        _pilot.reset(new Pilot(sc));
      }

      _pilot->addAction(ad);
      cout << "Added action for pilot of spacecraft " << spacecraftId << ": "
          << ad.toString(true) << endl;
      break;
    }
  }
}

