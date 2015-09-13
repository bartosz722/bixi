#include <iostream>
#include <cmath>
#include "DataLoader.h"
#include "SphericalObject.h"
#include "PhysicalDefinitions.h"
#include "PrecisionTester.h"

using namespace std;

bool loadUniverseData(Universe & u) {
  int physObjSet = 6;

  Universe::Settings s;
  s._collision = Universe::CollisionBehaviour::StopUniverse;
  s._collisionTolerance = 0.00001;

  PhysicalObjectProperties pop;

  if(physObjSet == 1) {
    s._G = constG;
    s._timeUnit = 0.1;
    s._roundsPerSecond = 50;
    s._ticksPerRound = 200;

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
    someRock._mass = earth._mass / 10;
    someRock._position = Vector(earth._radius * 3, 0, 0);
    someRock._velocity = Vector(-500, 3000, 0);
    someRock._radius = earth._radius / 10;
    pop._color = Color();
    pop._tracked = true;
//    u.insertPhysicalObject(someRock, pop);
  }
  else if(physObjSet == 2) {
    // sunsystem.py
    s._G = 8;
    s._timeUnit = 0.01; // 0.01 - original
    s._roundsPerSecond = 50;
    s._ticksPerRound = 2;

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
    // twins
    s._G = 8;
    s._timeUnit = 0.01;
    s._roundsPerSecond = 50;
    s._ticksPerRound = 40;

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
    // orbit.py
    s._G = 1;
    s._timeUnit = 0.02; // 0.02 - original
    s._roundsPerSecond = 50;
    s._ticksPerRound = 5;

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
  else if(physObjSet == 5) {
    // collision test
    s._G = 0.05;
    s._timeUnit = 0.01;
    s._roundsPerSecond = 50;
    s._ticksPerRound = 150;
    s._collision = Universe::CollisionBehaviour::Inelastic;

    SphericalObject rock;
    rock._mass = 100;
    rock._position = Vector(100, 0, 0);
    rock._velocity = Vector(-1, 0, 0);
    rock._radius = 10;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(rock, pop);

    SphericalObject rock2;
    rock2._mass = 200;
    rock2._position = Vector(-100, 0, 0);
    rock2._velocity = Vector(1, 0.03, 0);
    rock2._radius = 20;
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(rock2, pop);
  }
  else if(physObjSet == 6) {
    // frustum test
    s._G = constG;
    s._timeUnit = 0.1;
    s._roundsPerSecond = 50;
    s._ticksPerRound = 200;

    SphericalObject earth;
    earth._radius = 6370*1000;
    earth._mass = 5.972 * pow(10, 24);
    earth._position = Vector(0, 0, -(earth._radius * 2));
    earth._velocity = Vector(0, 0, 0);
    pop._color = { 47, 183, 0 };
    pop._tracked = true;
    u.insertPhysicalObject(earth, pop);

    SphericalObject iss;
    iss._radius = 1000 * 300;
    iss._mass = 417289;

    double a = (earth._radius + 430*1000);
    double angle = (70 / 180.0 * M_PI);
    double x = a * cos(angle);
    double z = a * sin(angle);
    iss._position = Vector(x, 0, z + earth._position.v[2]);

    iss._velocity = Vector(0, 7706, 0);
    pop._color = Color();
    pop._tracked = true;
    u.insertPhysicalObject(iss, pop);

    SphericalObject rock;
    rock._mass = 5.972 * pow(10, 24);
    rock._position = Vector(0, 0, -(3));
    rock._velocity = Vector(0, 1000 * 2, -0.05);
    rock._radius = 1000 * 300;
    pop._color = { 47, 183, 0 };
    pop._tracked = true;
//    u.insertPhysicalObject(rock, pop);
  }

  u.setSettings(s);

  return true;
}

void loadPrecisionTestData(Universe & u) {
  PrecisionTester::Settings pts;
  pts._orbitsToDo = 10;
  pts._timeUnit = 0.01;
  pts._G = 8;
  pts._x = 100;
  pts._velocityY = 10;
  pts._mass = 12500.0;
  pts._roundsPerSecond = 50;
  pts._ticksPerRound = 50;
  u.setUpPrecisionTest(pts);
}

