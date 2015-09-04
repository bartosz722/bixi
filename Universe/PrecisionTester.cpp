#include <cmath>
#include <PrecisionTester.h>
#include "SphericalObject.h"

PrecisionTester::PrecisionTester(const PhysicalObjectsContainer & poc)
  : _objects(poc)
  , _precisionTestObjectLastY(0.0)
{
}

void PrecisionTester::setUp(const Settings & s) {
  _settings = s;

  _result._rightXRange =  {_settings._x, _settings._x};
  _result._leftXRange =  {0, 0};
  _result._rightXRangeDeviationPercentage = {0, 0};
  _result._leftXRangeDiff = 0;
  _result._leftXRangeDiffPercentage = 0;
  _result._orbitCount = 0;

  _precisionTestObjectLastY = 0.0;
}

PrecisionTester::TestObjects PrecisionTester::getTestObjects() {
  TestObjects to;
  SphericalObject planet;

  planet._mass = _settings._mass;
  planet._position = Vector(_settings._x, 0, 0);
  planet._velocity = Vector(0, _settings._velocityY, 0);
  planet._radius = _settings._x / 100;
  to.push_back(planet.copy());

  planet._position.v[0] *= -1.0;
  planet._velocity.v[1] *= -1.0;
  to.push_back(planet.copy());

  return to;
}

bool PrecisionTester::precisionTestTick() {
  const PhysicalObject & to = *_objects[0];
  const double currX = to._position.v[0];
  const double currY = to._position.v[1];

  if(currY > 0.0 && _precisionTestObjectLastY < 0.0) {
    if(currX < _result._rightXRange.first) {
      _result._rightXRange.first = currX;
    }
    if(currX > _result._rightXRange.second) {
      _result._rightXRange.second = currX;
    }
    ++_result._orbitCount;
  }
  else if(currY < 0.0 && _precisionTestObjectLastY > 0.0) {
    if(_result._orbitCount == 0) { // uninitialized
      _result._leftXRange.first = currX;
      _result._leftXRange.second = currX;
    }
    if(currX < _result._leftXRange.first) {
      _result._leftXRange.first = currX;
    }
    if(currX > _result._leftXRange.second) {
      _result._leftXRange.second = currX;
    }
  }

  _precisionTestObjectLastY = currY;

  if(_result._orbitCount >= _settings._orbitsToDo) {
    return false;
  }

  return true;
}

PrecisionTester::Result PrecisionTester::getPrecisionTestResult() {
  _result._rightXRangeDeviation.first =
      fabs(_settings._x - _result._rightXRange.first);
  _result._rightXRangeDeviation.second =
      fabs(_settings._x - _result._rightXRange.second);

  _result._rightXRangeDeviationPercentage.first =
      _result._rightXRangeDeviation.first / _settings._x * 100.0;
  _result._rightXRangeDeviationPercentage.second =
      _result._rightXRangeDeviation.second / _settings._x * 100.0;

  _result._leftXRangeDiff =
      fabs(_result._leftXRange.first -
           _result._leftXRange.second);
  _result._leftXRangeDiffPercentage =
      _result._leftXRangeDiff / _settings._x * 100.0;

  return _result;
}
