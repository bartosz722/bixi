#include "GlVectorConversion.h"

glm::dvec3 vectorToGlmDvec3(const Vector & v) {
  return glm::dvec3(v.v[0], v.v[1], v.v[2]);
}

