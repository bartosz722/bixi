/*
 * Pilot.cpp
 *
 *  Created on: 20 gru 2015
 *      Author: bsp
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <Pilot.h>
#include <Assert.h>

using namespace std;

namespace {
std::string actionToString(Pilot::Action a) {
  switch(a) {
  case Pilot::Action::EngineOn:
    return "Engine on";
  case Pilot::Action::EngineOff:
    return "Engine off";
  default:
    break;
  }

  return to_string((int)a);
}
}

bool Pilot::ActionData::operator<(const ActionData & other) const {
  if(_execTime == other._execTime) {
    return _order < other._order;
  }
  else {
    return _execTime < other._execTime;
  }
}

Pilot::Pilot(Spacecraft & s)
  : _actionsSorted(false)
  , _spacecraft(s)
{
}

void Pilot::addAction(const ActionData & a) {
  ASSERT(!_actionsSorted);
  _actions.push_back(a);
}

void Pilot::executeCurrentActions(double currentTime) {
  if(!_actionsSorted) {
    sortActions();
  }

  ActionData actionData;
  while(getActionToExecute(actionData, currentTime)) {
    cout << "Time " << currentTime << ": exexuting action "
        << actionToString(actionData._action) << endl;

    switch(actionData._action) {
    case Action::EngineOn:
      _spacecraft._engineOn = true;
      break;
    case Action::EngineOff:
      _spacecraft._engineOn = false;
      break;
    default:
      cout << "Unknown action: " << actionToString(actionData._action) << endl;
    }
  }
}

bool Pilot::getActionToExecute(ActionData & actionData, double currentTime) {
  if(_actions.empty()) {
    return false;
  }

  const ActionData & first = _actions.front();
  if(currentTime >= first._execTime) {
    actionData = first;
    _actions.pop_front();
    return true;
  }

  return false;
}

void Pilot::sortActions() {
  ASSERT(!_actionsSorted);
  std::sort(_actions.begin(), _actions.end());
  _actionsSorted = true;
}
