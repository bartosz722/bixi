#include "Tracker.h"
#include "Assert.h"

Tracker::Tracker(size_t density, size_t capacity)
  : _density(density), _capacity(capacity), _pushDataCount(0)
{
  ASSERT(_density > 0);
  ASSERT(_capacity > 0);
}

void Tracker::trackObject(int id) {
  ASSERT(_tracks.count(id) == 0);
  _tracks[id] = Track(_capacity);
}

void Tracker::pushData(const PhysicalObjectsContainer& poc) {
  if(_pushDataCount++ % _density == 0) {
    // Add new elements
    for(const auto & po : poc) {
      auto fi = _tracks.find(po->getId());
      if(fi != _tracks.end() && po->_active) {
        fi->second.push_back(po->_position);
      }
    }
  }
}

size_t Tracker::getSizeOfLongestTrack() const {
  size_t max = 0;
  for(auto & t : _tracks) {
    if(t.second.size() > max) {
      max = t.second.size();
    }
  }
  return max;
}
