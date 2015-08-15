#include <GL/glut.h>
#include <cmath>

void drawFilledCircle(double centerX, double centerY, double radius, int parts) {
  glPushMatrix();
  glTranslated(centerX, centerY, 0);

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
