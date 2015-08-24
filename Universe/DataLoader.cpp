#include <iostream>
#include <cmath>
#include "DataLoader.h"
#include "SphericalObject.h"

using namespace std;

bool loadSettings(Universe & u) {
  Universe::Settings s;

  s._timeUnit = 1.0;
  s._detectCollision = true;
  s._collisionTolerance = 0.00001;
  s._roundsPerSecond = 50;
  s._ticksPerRound = 20;

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
    earth._velocity = Vector(0, -3000, 0);
    earth._radius = 6370*1000;
    pop._color = { 77, 100, 20 };
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
//    u.insertPhysicalObject(iss, pop);

    SphericalObject someRock;
    someRock._mass = earth._mass;
    someRock._position = Vector(earth._radius * 3, 0, 0);
    someRock._velocity = Vector(0, 3000, 0);
    someRock._radius = earth._radius;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(someRock, pop);
  }
  else if(physObjSet == 2) {
    //G=8;

    SphericalObject sun;
    sun._mass = 1000000;
    sun._position = Vector(0, 0, 0);
    sun._velocity = Vector(0, -2.5, 0);
    sun._radius = 1.8;
    pop._color = Color();
    pop._tracked = false;
    u.insertPhysicalObject(sun, pop);

    SphericalObject earth;
    earth._mass = 12500;
    earth._position = Vector(210, 0, 0);
    earth._velocity = Vector(0, 195, 0);
    earth._radius = 0.8;
    pop._color = Color();
    pop._tracked = false;
    u.insertPhysicalObject(earth, pop);

    SphericalObject moon;
    moon._mass = 1;
    moon._position = Vector(220, 0, 0);
    moon._velocity = Vector(0, 295, 0);
    moon._radius = 0.5;
    pop._color = Color();
    pop._tracked = false;
    u.insertPhysicalObject(moon, pop);
  }

  return true;
}

