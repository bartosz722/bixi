#ifndef PHYSICALOBJECTSCONTAINER_H_
#define PHYSICALOBJECTSCONTAINER_H_

#include <vector>

#include "BasicDefinitions.h"

class PhysicalObject;

class PhysicalObjectsContainer {
public:
  typedef std::vector<PhysicalObject *> ContainterT;
  typedef ContainterT::const_iterator IteratorT;

  PhysicalObjectsContainer();
  ~PhysicalObjectsContainer();

  void clear();
  int count() const;
//  TickT getTick() const { return _tick; }
//  double getElapsedTime() const { return _elapsedTime; }
  void append(PhysicalObject * po); // c++11 unique_ptr
  void operator=(const PhysicalObjectsContainer & other);
  bool isCompatibleWith(const PhysicalObjectsContainer & other) const;

//  // iteration:
//  void beginIteration();
//  bool iterationEnded();
//  PhysicalObject & currentObject();

  TickT _tick;
  double _elapsedTime; // s
  ContainterT _objects; // c++11 - unique_ptr

private:
//  typedef std::vector<PhysicalObject *> ContainterT;
//  ContainterT::const_iterator _iterator;
  PhysicalObjectsContainer(PhysicalObjectsContainer & other) {} // c++11 delete
};

#endif /* PHYSICALOBJECTSCONTAINER_H_ */
