#include <cmath>

#include "Vector.h"

bool vectorsEqual(const Vector & v1, const Vector & v2, double tolerance) {
  return fabs(v1.v[0] - v2.v[0]) <= tolerance &&
         fabs(v1.v[1] - v2.v[1]) <= tolerance &&
         fabs(v1.v[2] - v2.v[2]) <= tolerance;
}
