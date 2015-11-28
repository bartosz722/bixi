#include <iostream>
#include <GL/glu.h>
#include "Texture.h"
#include "ImageData.h"

using namespace std;

Texture::Texture()
  : _idValid(false)
  , _id(0)
{
}

void Texture::destroy() {
  if(_idValid) {
    glDeleteTextures(1, &_id);
    cout << "texure deleted: " << _id << endl;
    _idValid = false;
    _id = 0;
  }
}

void Texture::createTexture() {
  glGenTextures(1, &_id);
  _idValid = true;
  glBindTexture(GL_TEXTURE_2D, _id);
  cout << "texure created: " << _id << endl;
}

bool Texture::createOneColor(GLubyte r, GLubyte g, GLubyte b) {
  if(_idValid) {
    cout << "Texture reloading is not supported\n";
    return false;
  }

  createTexture();

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLubyte texData [] = { r, g, b };
  auto buildRet = gluBuild2DMipmaps(
      GL_TEXTURE_2D, 3, 1, 1,
      GL_RGB, GL_UNSIGNED_BYTE, texData);
  if(buildRet != 0) {
    destroy();
    return false;
  }

  return true;
}

bool Texture::createFromImage(const char * filename) {
  if(_idValid) {
    cout << "Texture reloading is not supported\n";
    return false;
  }

  if(!setupTextureAndLoadFile(filename)) {
    cout << "Failed to load texture from " << filename << endl;
    destroy();
    return false;
  }

  cout << "Texture loaded from " << filename << endl;

  return true;
}

bool Texture::setupTextureAndLoadFile(const char * filename) {
  createTexture();

  // Texture resize options
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR );

  // Texture wrapping
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  ImageData imgd;
  imgd.loadJpegFile(filename);
  if(imgd.getRGBData() == NULL) {
    return false;
  }

  auto buildRet = gluBuild2DMipmaps(
      GL_TEXTURE_2D, 3, imgd.getWidth(), imgd.getHeight(),
      GL_RGB, GL_UNSIGNED_BYTE, imgd.getRGBData());
  if(buildRet != 0) {
    return false;
  }

  return true;
}

void Texture::use() const {
  if(_idValid) {
    glBindTexture(GL_TEXTURE_2D, _id);
  }
}
