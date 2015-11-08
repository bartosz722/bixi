#ifndef UNIVERSE_PHYSICALOBJECTPROPERTIES_H_
#define UNIVERSE_PHYSICALOBJECTPROPERTIES_H_

#include "PhysicalObject.h"
#include "Color.h"

class PhysicalObjectProperties {
public:
  PhysicalObjectProperties();

  void setId(int id);
  int getId() const { return _id; }

  Color _color;
  bool _tracked;
  const char * _texture;

private:
  int _id;
};

#endif /* UNIVERSE_PHYSICALOBJECTPROPERTIES_H_ */
