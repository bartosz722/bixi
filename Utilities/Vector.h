#ifndef VECTOR_H_
#define VECTOR_H_

#include <ostream>

class Vector {
public:
  Vector();
  Vector(double x, double y, double z);

  void clear(); // TODO: unit test :)
  bool operator==(const Vector & other) const;
  Vector operator+(const Vector & other) const;
  Vector operator-(const Vector & other) const;
  Vector operator*(double factor) const;
  Vector operator/(double divisor) const;
  double length() const;
  double length_pow2() const;

  double v[3];

  friend std::ostream & operator<<(std::ostream & s, const Vector & v);
};

#endif /* VECTOR_H_ */
