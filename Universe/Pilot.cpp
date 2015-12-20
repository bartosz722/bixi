/*
 * Pilot.cpp
 *
 *  Created on: 20 gru 2015
 *      Author: bsp
 */

#include <algorithm>
#include <Pilot.h>
#include <Assert.h>

bool Pilot::ActionData::compare(const ActionData & ad1, const ActionData & ad2) {
  return ad1._execTime < ad2._execTime;
}

Pilot::Pilot(Spacecraft & s)
  : _actionsSorted(false)
  , _spacecraft(s)
{
}

void Pilot::addAction(double executionTime, Action a, double arg1) {
  ASSERT(!_actionsSorted);
  _actions.push_back({executionTime, a, arg1});
}

void Pilot::executeCurrentActions(double currentTime) {
  if(_actionsSorted) {
    sortActions();
  }


}

void Pilot::sortActions() {
  std::sort(_actions.begin(), _actions.end(), ActionData::compare);

  _actionsSorted = true;
}
