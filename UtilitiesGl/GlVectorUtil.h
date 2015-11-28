#include <glm/glm.hpp>
#include "Vector.h"

glm::dvec3 vectorToGlmDvec3(const Vector & v);

void getDirectionDifference(
    const glm::dvec3 & originalDirection, const glm::dvec3 & destinationDirection,
    double & rotationAngleDeg, glm::dvec3 & rotationAxis);

