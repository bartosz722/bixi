#include <iostream>
#include <jpeglib.h>
#include <stdio.h>
#include <ImageData.h>
#include <ScopedFile.h>

ImageData::ImageData() {
  _data = nullptr;
  _width = _height = 0;
}

ImageData::~ImageData() {
  clearData();
}

void ImageData::clearData() {
  delete [] _data;
  _data = nullptr;
  _width = _height = 0;
}

void ImageData::loadJpegFile(const char * filename) {
  clearData();

  ScopedFile infile(fopen(filename, "rb"));
  if(!infile.getFile()) {
    std::cout << "Error opening jpeg file: " << filename << std::endl;
    return;
  }

  if(!readJpegData(infile.getFile())) {
    clearData();
    return;
  }
}

bool ImageData::readJpegData(FILE * infile) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  if(jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
    std::cout << "Error reading jpeg header" << std::endl;
    return false;
  }

  if(cinfo.num_components != 3) {
    std::cout << "Error: jpeg num_components is " << cinfo.num_components << std::endl;
    return false;
  }

  if(!jpeg_start_decompress(&cinfo)) {
    std::cout << "Error in ImageData::loadJpegFile(): " << 1 << std::endl;
  }

  _data = new uint8_t [cinfo.output_width * cinfo.output_height * cinfo.num_components];
  _width = cinfo.output_width;
  _height = cinfo.output_height;

  size_t location = cinfo.output_width * cinfo.num_components * (cinfo.image_height - 1);

  while(cinfo.output_scanline < cinfo.image_height) {
    JSAMPROW row_pointer = _data + location;
    if(jpeg_read_scanlines(&cinfo, &row_pointer, 1) != 1) {
      std::cout << "Error in ImageData::loadJpegFile(): " << 2 << std::endl;
    }
    location -= cinfo.output_width * cinfo.num_components;
  }

  if(!jpeg_finish_decompress(&cinfo)) {
    std::cout << "Error in ImageData::loadJpegFile(): " << 3 << std::endl;
    return false;
  }

  jpeg_destroy_decompress(&cinfo);

  return true;
}
