#ifndef UTILITIES_COLOR_H_
#define UTILITIES_COLOR_H_

#include <cstdint>

class Color {
public:
  Color() {
    _c[0] = _c[1] = _c[2] = 0;
    _valid = false;
  }

  Color(uint8_t ar, uint8_t ag, uint8_t ab) {
    _c[0] = ar;
    _c[1] = ag;
    _c[2] = ab;
    _valid = true;
  }

  bool isValid() const { return _valid; }
  const uint8_t * rgbData() const { return _c; }

private:
  uint8_t _c[3]; // RGB
  bool _valid;
};

#endif /* UTILITIES_COLOR_H_ */
