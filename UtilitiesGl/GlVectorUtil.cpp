#include "GlVectorUtil.h"

glm::dvec3 vectorToGlmDvec3(const Vector & v) {
  return glm::dvec3(v.v[0], v.v[1], v.v[2]);
}

// Vectors originalDirection and destinationDirection must be normalized.
void getDirectionDifference(
    const glm::dvec3 & originalDirection, const glm::dvec3 & destinationDirection,
    double & rotationAngleDeg, glm::dvec3 & rotationAxis)
{
  if(originalDirection == destinationDirection) {
    rotationAxis = glm::dvec3(0, 1, 0);
    rotationAngleDeg = 0;
    return;
  }

  if(originalDirection == -destinationDirection) {
    rotationAxis = glm::dvec3(0, 1, 0);
    rotationAngleDeg = 180;
    return;
  }

  rotationAxis = glm::cross(originalDirection, destinationDirection);

  auto dotProduct = glm::dot(originalDirection, destinationDirection);
  auto rotationAngleRad = acos(dotProduct);
  rotationAngleDeg = rotationAngleRad / M_PI * 180.0;
}
