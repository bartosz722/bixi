#include <cmath>
#include "Vector.h"

Vector::Vector() {
  for(int i=0; i<3; ++i) {
    v[i] = 0.0;
  }
}

Vector::Vector(double x, double y, double z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

bool Vector::operator==(const Vector & other) const {
  for(int i=0; i<3; ++i) {
    if(v[i] != other.v[i]) {
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

std::ostream & operator<<(std::ostream & s, const Vector & v) {
  s << "[" << v.v[0] << ", " << v.v[1] << ", " << v.v[2] << "]";
  return s;
}
