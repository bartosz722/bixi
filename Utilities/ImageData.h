#ifndef UTILITIES_IMAGEDATA_H_
#define UTILITIES_IMAGEDATA_H_

#include <cstdint>

class ImageData {
public:
  ImageData();
  ~ImageData();

  // Returns not NULL when image was successfully loaded
  uint8_t * getRGBData() const { return _data; }

  size_t getWidth() const { return _width; }
  size_t getHeight() const { return _height; }

  void  loadJpegFile(const char * filename);

private:
  void clearData();
  bool readJpegData(FILE * infile);

  uint8_t * _data; // RGB; data size is width * height * 3
  size_t _width, _height;
};

#endif /* UTILITIES_IMAGEDATA_H_ */
