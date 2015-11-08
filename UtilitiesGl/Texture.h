#ifndef UTILITIESGL_TEXTURE_H_
#define UTILITIESGL_TEXTURE_H_

#include <GL/gl.h>

class Texture {
public:
  Texture();

  bool createFromImage(const char * filename);
  void destroy();

  void use();

private:
  bool setupTextureAndLoadFile(const char * filename);

  bool _idValid;
  GLuint _id;

  Texture(Texture &) = delete;
  void operator=(Texture &) = delete;
};

#endif /* UTILITIESGL_TEXTURE_H_ */
