#ifndef UTILITIES_SCOPEDFILE_H_
#define UTILITIES_SCOPEDFILE_H_

#include <stdio.h>

class ScopedFile {
public:
  ScopedFile(FILE * f) : _f(f) {}

  ~ScopedFile() {
    if(_f) {
      fclose(_f);
    }
  }

  FILE * getFile() const { return _f; }

private:
  FILE * _f;
};

#endif /* UTILITIES_SCOPEDFILE_H_ */
