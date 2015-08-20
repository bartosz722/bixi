#include "Tracker.h"
#include "Assert.h"

Tracker::Tracker(std::size_t density, std::size_t capacity)
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
      if(fi != _tracks.end()) {
        fi->second.push_back(po->_position);
      }
    }
  }
}

const Tracker::Track * Tracker::getTrack(int id) const {
  auto fi = _tracks.find(id);
  if(fi == _tracks.end()) {
    return nullptr;
  }
  return &(fi->second);
}
