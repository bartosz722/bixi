#ifndef UNIVERSE_PRECISIONTESTER_H_
#define UNIVERSE_PRECISIONTESTER_H_

#include <vector>
#include <memory>
#include "PhysicalObject.h"
#include "PhysicalObjectsContainer.h"

class PrecisionTester {
public:
  struct Settings {
    size_t _orbitsToDo;
    double _timeUnit; // s; tick time unit
    double _G;
    double _x;
    double _velocityY;
    double _mass;
    size_t _roundsPerSecond; // set to 0 for no limitation
    size_t _ticksPerRound; // set to 0 for no limitation
  };

  struct Result {
    std::pair<double, double> _rightXRange; // at y~=0
    std::pair<double, double> _leftXRange; // at y~=0
    std::pair<double, double> _rightXRangeDeviation;
    std::pair<double, double> _rightXRangeDeviationPercentage;
    double _leftXRangeDiff;
    double _leftXRangeDiffPercentage;
    size_t _orbitCount;
  };

  typedef std::vector<std::unique_ptr<PhysicalObject>> TestObjects;

  PrecisionTester(const PhysicalObjectsContainer & poc);
  void setUp(const Settings & s);
  TestObjects getTestObjects(); // returns objects that should be put to _objects
  bool precisionTestTick(); // returns: true - continue testing, false - test finished
  Result getPrecisionTestResult();

private:
  const PhysicalObjectsContainer & _objects;
  Settings _settings;
  Result _result;
  double _precisionTestObjectLastY;

  // Synchronization:
  // Fields _objects and _result must be synchronized externally by the same mutex.
  // As a result calls to precisionTestTick() and getPrecisionTestResult() are included
  // in this synchronization.
};

#endif /* UNIVERSE_PRECISIONTESTER_H_ */
