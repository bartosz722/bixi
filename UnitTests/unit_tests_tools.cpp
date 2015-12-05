#include <cmath>
#include "unit_tests_tools.h"

namespace ut_tools {

bool compareVectors(const Vector & v1, const Vector & v2, double tolerance) {
  for(int i=0; i<3; ++i) {
    if(fabs(v1.v[i] - v2.v[i]) > tolerance) {
      return false;
    }
  }

  return true;
}

}
