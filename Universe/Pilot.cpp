/*
 * Pilot.cpp
 *
 *  Created on: 20 gru 2015
 *      Author: bsp
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <Pilot.h>
#include <Assert.h>

using namespace std;

bool Pilot::ActionData::operator<(const ActionData & other) const {
  if(_execTime == other._execTime) {
    return _order < other._order;
  }
  else {
    return _execTime < other._execTime;
  }
}

std::string Pilot::ActionData::toString(bool withOrderData) const {
  ostringstream ss;

  ss << "action '";
  switch(_action) {
  case Action::EngineOn: ss << "Engine on"; break;
  case Action::EngineOff: ss << "Engine off"; break;
  case Action::SetThrustMassRate: ss << "Set thrust mass rate"; break;
  case Action::SetThrustSpeed: ss << "Set thrust speed"; break;
  case Action::TurnRight: ss << "Turn right"; break;
  case Action::TurnUp: ss << "Turn up"; break;
  default:
    ss << "Unknown (" << (int)_action << ")";
  }
  ss << "'";

  if(_action >= Action::SetThrustMassRate && _action <= Action::TurnUp) {
    ss << ", arg1: " << _arg1;
  }

  if(withOrderData) {
    ss << ", time: " << _execTime << ", order " << _order;
  }

  return ss.str();
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
    cout << "Time " << currentTime << ": executing " << actionData.toString() << endl;

    switch(actionData._action) {
    case Action::EngineOn:
      _spacecraft._engineOn = true;
      break;
    case Action::EngineOff:
      _spacecraft._engineOn = false;
      break;
    case Action::SetThrustMassRate:
      _spacecraft._thrustMassRate = actionData._arg1;
      break;
    case Action::SetThrustSpeed:
      _spacecraft._thrustSpeed = actionData._arg1;
      break;
    case Action::TurnRight:
      _spacecraft.turnFromLeftToRight(actionData._arg1);
      break;
    case Action::TurnUp:
      _spacecraft.turnFromDownToUp(actionData._arg1);
      break;
    default:
      cout << "Unknown action!" << endl;
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
