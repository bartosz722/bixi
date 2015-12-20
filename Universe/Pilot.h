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

  Pilot(Spacecraft & s);

  void addAction(double executionTime, Action a, double arg1);
  void executeCurrentActions(double currentTime);

private:
  struct ActionData {
      double _execTime;
      Action _action;
      double _arg1;

      static bool compare(const ActionData & ad1, const ActionData & ad2);
    };

  void sortActions();

  std::deque<ActionData> _actions;
  bool _actionsSorted;

  Spacecraft & _spacecraft;
};

#endif /* UNIVERSE_PILOT_H_ */
