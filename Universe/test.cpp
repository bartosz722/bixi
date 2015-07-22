#include "PhysicalObject.h"
#include "SphericalObject.h"
#include "PhysicalDefinitions.h"
#include "BasicDefinitions.h"

#include <iostream>
#include <cmath>

using namespace std;

void test() {
  {
    PhysicalObject pl1, pl2;
    pl1._mass = 8 * pow(10, 10);
    pl2._mass = 11 * pow(10, 10);
    pl1._position = Vector(1000, 1000, 1000);
    pl2._position = Vector(2000, 2000, 2000);
    Vector gForce1 = pl1.getGravityForce(pl2);
    Vector gForce2 = pl2.getGravityForce(pl1);
    Vector gForce0 = pl2.getGravityForce(pl2);

    cout << "g force 1: " << gForce1 << endl;
    cout << "g force 2: " << gForce2 << endl;
    cout << "g force 0: " << gForce0 << endl;
  }

  {
    PhysicalObject czlowiek;
    SphericalObject ziemia;
    ziemia._mass = 5.97219 * pow(10, 24);
    czlowiek._mass = 70;
    czlowiek._position = Vector(6371000, 0, 0);
    Vector silaNaCzlowiekaV = czlowiek.getGravityForce(ziemia);
    cout << "silaNaCzlowiekaV: " << silaNaCzlowiekaV << endl;
    double silaNaCzlowieka = silaNaCzlowiekaV.length();
    cout << "silaNaCzlowieka: " << silaNaCzlowieka << endl;
    double g = silaNaCzlowieka / czlowiek._mass;
    cout << "g: " << g << endl;

    cout << "types: " << enumValue(czlowiek.getType())
        << ", " << enumValue(ziemia.getType()) << endl;
  }

}
