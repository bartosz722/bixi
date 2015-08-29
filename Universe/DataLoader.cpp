#include <iostream>
#include <cmath>
#include "DataLoader.h"
#include "SphericalObject.h"
#include "PhysicalDefinitions.h"

using namespace std;

bool loadSettings(Universe & u) {
  Universe::Settings s;

  s._G = constG;
  s._timeUnit = 0.01;
  s._roundsPerSecond = 50;
  s._ticksPerRound = 1000;

  s._detectCollision = true;
  s._collisionTolerance = 0.00001;

  u.setSettings(s);
  return true;
}

bool loadPhysicalObjects(Universe & u) {
  int physObjSet = 1;

  PhysicalObjectProperties pop;

  if(physObjSet == 1) {
    SphericalObject earth;
    earth._mass = 5.972 * pow(10, 24);
    earth._position = Vector(0, 0, 0);
    earth._velocity = Vector(0, 0, 0);
    earth._radius = 6370*1000;
    pop._color = { 47, 183, 0 };
    pop._tracked = true;
    u.insertPhysicalObject(earth, pop);

    PhysicalObject man1;
    man1._mass = 70;
    man1._position = Vector(earth._radius + 10, 0, 0);
    man1._velocity = Vector(0, 0, 0);
    pop._color = Color();
    pop._tracked = true;
//    u.insertPhysicalObject(man1, pop);

    PhysicalObject iss;
    iss._mass = 417289;
    iss._position = Vector(earth._radius + 430*1000, 0, 0);
    iss._velocity = Vector(0, 7706, 0);
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(iss, pop);

    SphericalObject someRock;
    someRock._mass = earth._mass;
    someRock._position = Vector(earth._radius * 3, 0, 0);
    someRock._velocity = Vector(0, 3000, 0);
    someRock._radius = earth._radius;
    pop._color = Color();
    pop._tracked = true;
//    u.insertPhysicalObject(someRock, pop);
  }
  else if(physObjSet == 2) {
    //sunsystem.py
    //G=8;
    //t=0.01 - original

    SphericalObject sun;
    sun._mass = 1000000;
    sun._position = Vector(0, 0, 0);
    sun._velocity = Vector(0, -2.5, 0);
    sun._radius = 20;
    pop._color = Color();
    pop._tracked = false;
    u.insertPhysicalObject(sun, pop);

    SphericalObject earth;
    earth._mass = 12500;
    earth._position = Vector(210, 0, 0);
    earth._velocity = Vector(0, 195, 0);
    earth._radius = 5;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(earth, pop);

    SphericalObject moon;
    moon._mass = 1;
    moon._position = Vector(220, 0, 0);
    moon._velocity = Vector(0, 295, 0);
    moon._radius = 1;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(moon, pop);
  }
  else if(physObjSet == 3) {
    //G=8;

    SphericalObject earth;
    earth._mass = 12500;
    earth._position = Vector(-100, 0, 0);
    earth._velocity = Vector(0, 10, 0);
    earth._radius = 5;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(earth, pop);

    earth._position = Vector(100, 0, 0);
    earth._velocity = Vector(0, -10, 0);
    u.insertPhysicalObject(earth, pop);
  }
  else if(physObjSet == 4) {
    //G=1;
    //dt=0.02 - original
    //optimal view: 50 ticks/s for dt=0.02

    SphericalObject earth;
    earth._mass = 2.5;
    earth._position = Vector(-1, 0.20, 0);
    earth._velocity = Vector(0, 0.2, -0.3);
    earth._radius = 0.075;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(earth, pop);

    SphericalObject rock;
    rock._mass = 1;
    rock._position = Vector(1, 0, 0);
    rock._velocity = earth._velocity * earth._mass / rock._mass * -1.0;
    rock._radius = 0.04;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(rock, pop);
  }

  return true;
}

void loadPrecisionTestData(Universe & u) {
  Universe::PrecisionTestData ptd;
  ptd._orbitsToDo = 10;
  ptd._timeUnit = 0.01;
  ptd._G = 8;
  ptd._x = 100;
  ptd._velocityY = 10;
  ptd._mass = 12500.0;
  ptd._roundsPerSecond = 50;
  ptd._ticksPerRound = 50;
  u.setPrecisionTestData(ptd);
}

