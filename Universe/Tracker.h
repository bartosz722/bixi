#ifndef TRACKER_H_
#define TRACKER_H_

#include <map>
#include <deque>
#include "PhysicalObjectsContainer.h"
#include "Vector.h"

class Tracker {
public:
  typedef std::deque<Vector> Track;

  // Takes data from 1 in 'density' pushData() calls.
  // 'capacity' - max elements in one track
  Tracker(std::size_t density, std::size_t capacity);
  void trackObject(int id);
  void pushData(const PhysicalObjectsContainer & poc);
  const Track * getTrack(int id) const;

private:
  static const int _oldToRemoveDivisor = 10;

  const std::size_t _density;
  const std::size_t _capacity;
  std::size_t _pushDataCount;
  std::map<int, Track> _tracks;
};

#endif
