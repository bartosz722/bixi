/*
 * Pilot.h
 *
 *  Created on: 20 gru 2015
 *      Author: bsp
 */

#ifndef UNIVERSE_PILOT_H_
#define UNIVERSE_PILOT_H_

#include <deque>
#include <Spacecraft.h>

class Pilot {
public:
  enum class Action {
    EngineOn,
    EngineOff,
    SetThrustMassRate,
    SetThrustSpeed,
    TurnRight,
    TurnUp
  };

  struct ActionData {
    double _execTime;
    uint16_t _order; // lower value - first to execute
    Action _action;
    double _arg1;

    bool operator<(const ActionData & other) const;
  };

  Pilot(Spacecraft & s);

  void addAction(const ActionData & a);
  void executeCurrentActions(double currentTime);

private:
  void sortActions();
  bool getActionToExecute(ActionData & actionData, double currentTime);

  std::deque<ActionData> _actions;
  bool _actionsSorted;

  Spacecraft & _spacecraft;
};

#endif /* UNIVERSE_PILOT_H_ */
