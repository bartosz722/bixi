#include <cmath>
#include "Math.h"
#include "Vector.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace {

glm::dvec3 privVectorToGlmDvec3(const Vector & v) {
  return glm::dvec3(v.v[0], v.v[1], v.v[2]);
}

Vector privGlmDvec3ToVector(const glm::dvec3 & v) {
  return Vector(v.x, v.y, v.z);
}

}


Vector::Vector() {
  clear();
}

Vector::Vector(double x, double y, double z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

void Vector::clear() {
  for(int i=0; i<3; ++i) {
    v[i] = 0.0;
  }
}

bool Vector::operator==(const Vector & other) const {
  for(int i=0; i<3; ++i) {
    if(v[i] != other.v[i]) {
      return false;
    }
  }
  return true;
}

bool Vector::equals(const Vector & other, double tolerance) const {
  for(int i=0; i<3; ++i) {
    if(fabs(v[i] - other.v[i]) > tolerance) {
      return false;
    }
  }
  return true;
}

Vector Vector::operator+(const Vector & other) const {
  Vector ret;
  for(int i=0; i<3; ++i) {
    ret.v[i] = v[i] + other.v[i];
  }
  return ret;
}

Vector Vector::operator-(const Vector & other) const {
  Vector ret;
  for(int i=0; i<3; ++i) {
    ret.v[i] = v[i] - other.v[i];
  }
  return ret;
}

Vector Vector::operator*(double factor) const {
  Vector ret(*this);
  for(int i=0; i<3; ++i) {
    ret.v[i] *= factor;
  }
  return ret;
}

Vector Vector::operator/(double divisor) const {
  return operator*(1/divisor);
}

double Vector::length() const {
  return sqrt(length_pow2());
}

double Vector::length_pow2() const {
  double sum = 0.0;
  for(int i=0; i<3; ++i) {
    sum += v[i] * v[i];
  }
  return sum;
}

Vector Vector::crossProduct(const Vector & other) const {
  auto gv1 = privVectorToGlmDvec3(*this);
  auto gv2 = privVectorToGlmDvec3(other);
  auto gprod = glm::cross(gv1, gv2);
  return privGlmDvec3ToVector(gprod);
}

void Vector::rotate(const Vector & around, double angleDeg) {
  // When observer looks in direction of 'around' vector and angle is positive
  // then vector is rotated right.
  auto gthis = privVectorToGlmDvec3(*this);
  auto garound = privVectorToGlmDvec3(around);
  double angleRad = Math::degToRad(angleDeg);
  auto gresult = glm::rotate(gthis, angleRad, garound);
  *this = privGlmDvec3ToVector(gresult);
}

std::ostream & operator<<(std::ostream & s, const Vector & v) {
  s << "[" << v.v[0] << ", " << v.v[1] << ", " << v.v[2] << "]";
  return s;
}
