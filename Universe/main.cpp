#include <iostream>

#include "Universe.h"

using namespace std;

void test();

int main() {
  cout << "bixi" << endl;

//  test();

  Universe universe;

  if(universe.loadSettings()) {
    if(universe.loadPhysicalObjects()) {
      universe.start();
      universe.waitForFinish();
    }
  }

  return 0;
}
