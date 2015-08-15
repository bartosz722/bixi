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
  u.setSettings(s);
  return true;
}

bool loadPhysicalObjects(Universe & u) {
  int id = 1;

  {
    SphericalObject earth;
    earth.setId(id++);
    earth._mass = 5.972 * pow(10, 24);
    earth._position = Vector(0, 0, 0);
    earth._velocity = Vector(0, 0, 0);
    earth._radius = 6370*1000;

    PhysicalObject man1;
    man1.setId(id++);
    man1._mass = 70;
    man1._position = Vector(earth._radius + 10, 0, 0);
    man1._velocity = Vector(0, 0, 0);

    PhysicalObject iss;
    iss.setId(id++);
    iss._mass = 417289;
    iss._position = Vector(earth._radius + 430*1000, 0, 0);
    iss._velocity = Vector(0, 7706, 0);

    SphericalObject someRock;
    someRock.setId(id++);
    someRock._mass = earth._mass;
    someRock._position = Vector(earth._radius * 3, 0, 0);
    someRock._velocity = Vector(0, 3000, 0);
    someRock._radius = earth._radius;

    u.insertPhysicalObject(earth);
  //  u.insertPhysicalObject(man1));
    u.insertPhysicalObject(iss);
  //  u.insertPhysicalObject(someRock);
  }

//  {
//    //G=8;
//
//    SphericalObject sun;
//    sun.setId(id++);
//    sun._mass = 1000000;
//    sun._position = Vector(0, 0, 0);
//    sun._velocity = Vector(0, -2.5, 0);
//    sun._radius = 1.8;
//
//    SphericalObject earth;
//    earth.setId(id++);
//    earth._mass = 12500;
//    earth._position = Vector(210, 0, 0);
//    earth._velocity = Vector(0, 195, 0);
//    earth._radius = 0.8;
//
//    SphericalObject moon;
//    moon.setId(id++);
//    moon._mass = 1;
//    moon._position = Vector(220, 0, 0);
//    moon._velocity = Vector(0, 295, 0);
//    moon._radius = 0.5;
//
//    u.insertPhysicalObject(sun);
//    u.insertPhysicalObject(earth);
//    u.insertPhysicalObject(moon);
//  }

  Universe::Snapshot us;
  u.getSnapshot(us);
  cout << "objects loaded, count: " << us._objects.size() << endl;

  return true;
}

