#include <iostream>
#include <thread>

#include "Universe.h"

using namespace std;

void test();

void printPhysicalObjects(const PhysicalObjectsContainer & poc);

int main() {
  cout << "bixi" << endl;

//  test();

  Universe universe;
  Universe::Snapshot snapshot;

  if(!universe.loadSettings()) {
    cout << "Failed to load settings\n";
    exit(1);
  }
  if(!universe.loadPhysicalObjects()) {
    cout << "Failed to load physical objects\n";
    exit(1);
  }

  cout << "Initial state of objects:\n";
  universe.getSnapshot(snapshot);
  printPhysicalObjects(snapshot._objects);

  if(!universe.start()) {
    cout << "Failed to start Universe\n";
    exit(1);
  }


  for(int i=0; i<3; ++i) {
    this_thread::sleep_for(chrono::seconds(1));
    universe.getSnapshot(snapshot);
    cout << "loop: " << i << ", current tick: " << snapshot._currentTick << endl;
    printPhysicalObjects(snapshot._objects);
  }

  universe.stop();
  universe.waitForFinish();

  return 0;
}

void printPhysicalObjects(const PhysicalObjectsContainer & poc) {
  for(auto const & po : poc) {
    cout << "object " << po->getId()
        << ": position " << po->_position
        << ", velocity " << po->_velocity << " (abs " << po->_velocity.length() << ")"
        << endl;
  }
}

//if(collision) {
//  cout << "After tick " << _currentTick
//      << ": collision between object " << obj1.getId() << " and " << obj2.getId()
//      << ", distance " << distance << ", minDistance " << minDistance
//      << endl;
//}
