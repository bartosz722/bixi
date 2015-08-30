#include <cmath>
#include "Math.h"

namespace Math {

double ballVolume(double radius) {
  return 4.0 / 3.0 * M_PI * pow(radius, 3);
}

double ballRadius(double volume) {
  double a = 3 / (4 * M_PI) * volume;
  return cbrt(a);
}

}
