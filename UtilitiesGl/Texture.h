#ifndef UTILITIESGL_TEXTURE_H_
#define UTILITIESGL_TEXTURE_H_

#include <GL/gl.h>

class Texture {
public:
  Texture();

  bool createOneColor(GLubyte r, GLubyte g, GLubyte b);
  bool createFromImage(const char * filename);
  void destroy();

  bool isValid() const { return _idValid; }
  void use() const;

private:
  void createTexture();
  bool setupTextureAndLoadFile(const char * filename);

  bool _idValid;
  GLuint _id;
};

#endif /* UTILITIESGL_TEXTURE_H_ */
