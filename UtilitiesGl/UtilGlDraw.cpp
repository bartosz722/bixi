#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <cassert>
#include "UtilGlDraw.h"

void drawFilledCircle(double centerX, double centerY, double centerZ, double radius, int parts) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(centerX, centerY, centerZ);

  const double angleStep = 2 * M_PI / parts;
  double currAngle = angleStep;
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d(0, 0, 0);
  glVertex3d(radius, 0, 0);
  for(int i=0; i<parts; ++i) {
    double x = radius * cos(currAngle);
    double y = radius * sin(currAngle);
    glVertex3d(x, y, 0);
    currAngle += angleStep;
  }
  glEnd();

  glPopMatrix();
}

void drawCircle(double centerX, double centerY, double radius, int parts) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(centerX, centerY, 0);

  const double angleStep = 2 * M_PI / parts;
  double currAngle = angleStep;
  glBegin(GL_LINE_LOOP);
  glVertex3d(radius, 0, 0);
  for(int i=1; i<parts; ++i) {
    double x = radius * cos(currAngle);
    double y = radius * sin(currAngle);
    glVertex3d(x, y, 0);
    currAngle += angleStep;
  }
  glEnd();

  glPopMatrix();
}

void drawGlutSphere(double centerX, double centerY, double centerZ,
                    double radius, int slices, int stacks, bool solid) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(centerX, centerY, centerZ);

  if(solid) {
    glutSolidSphere(radius, slices, stacks);
  }
  else {
    glutWireSphere(radius, slices, stacks);
  }

  glPopMatrix();
}

void drawGluSphere(double centerX, double centerY, double centerZ,
                   double radius, int slices, int stacks, bool enableTexturing) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(centerX, centerY, centerZ);
  glRotated(-90, 1, 0, 0); // initial orientation

  GLUquadric * qu = gluNewQuadric();
  assert(qu != NULL);
  gluQuadricTexture(qu, enableTexturing ? GLU_TRUE : GLU_FALSE);
  gluSphere(qu, radius, slices, stacks);
  gluDeleteQuadric(qu);

  glPopMatrix();
}

// direction - normalized vector (length of 1)
void drawGluCylinder(const glm::dvec3 & center, const glm::dvec3 & direction,
                     double base, double top, double height,
                     int slices, int stacks, bool enableTexturing) {
  const double halfSize = height / 2;
  static const glm::dvec3 initialDirection(0, 0, 1); // initial direction of gluCylinder

  bool doRotation = false;
  glm::dvec3 rotationAxis(0, 0, 0);
  double rotationAngleDeg = 0;

  if(direction != initialDirection) {
    doRotation = true;

    if(direction == -initialDirection) {
      rotationAxis = glm::dvec3(0, 1, 0);
      rotationAngleDeg = 180;
    }
    else {
      rotationAxis = glm::cross(initialDirection, direction);
      // Both vectors have to be normalized otherwise angle computation will be invalid.
      auto dotProduct = glm::dot(initialDirection, direction);
      auto rotationAngleRad = acos(dotProduct);
      rotationAngleDeg = rotationAngleRad / M_PI * 180.0;
    }
  }

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslated(center.x, center.y, center.z);
  if(doRotation) {
    glRotated(rotationAngleDeg, rotationAxis.x, rotationAxis.y, rotationAxis.z);
  }
  glTranslated(0, 0, -halfSize);

  GLUquadric * qu = gluNewQuadric();
  assert(qu != NULL);
  gluQuadricTexture(qu, enableTexturing ? GLU_TRUE : GLU_FALSE);
  gluCylinder(qu, base, top, height, slices, stacks);
  gluDeleteQuadric(qu);

  glPopMatrix();

  /* gluCylinder():
   * Base of cylinder is on XY plane. Height raises in direction of Z axis
   */
}
