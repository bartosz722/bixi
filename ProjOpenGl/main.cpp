#include <iostream>
#include <thread>
#include <cmath>
#include <GL/glut.h>

#include "Universe.h"
#include "SphericalObject.h"
#include "UtilGlDraw.h"
#include "DataLoader.h"

using namespace std;

void printPhysicalObjects(const PhysicalObjectsContainer & poc);
void setupUniverse();
void setupOpenGL(int & argc, char **argv);
void paint();
void paintPhysicalObjects();
void reshape(int x, int y);
void readUniverse(int value);
const GLfloat * getColorForPhysicalObject(int index);

Universe universe;
Universe::Snapshot snapshot;
bool doPaintPhysicalObejcts = true;

const int windowWidth = 800;
const int windowHeight = 800;
const int repaintPeriodMs = 20;
const double initialViewBorder = 31850000;
const double physicalObjectSize = 0.03; // % of wiew width
const int lineWidthPix = 3;
const double viewBorderMargin = 0.03; // % of extreme coordinate

double currentViewBorder = initialViewBorder;

const GLfloat physObjColor[][3] = {
  { 0, 0, 1 }, // blue
  { 0.788f, 0, 0.804f }, // violet
  { 0.098f, 0.631f, 0 }, // green
  { 1, 0, 0 }, // red
};

int main(int argc, char **argv) {
  cout << "bixi OpenGL" << endl;

  setupUniverse();
  setupOpenGL(argc, argv);

  if(!universe.start()) {
    cout << "Failed to start Universe\n";
    exit(1);
  }
  // TODO: kończenie wątku przy zamykaniu aplikacji

  glutTimerFunc(0, readUniverse, 0);
  glutMainLoop();

  return 0;
}

void printPhysicalObjects(const PhysicalObjectsContainer & poc) {
  for(auto const & po : poc) {
    cout << "object " << po->getId()
        << ": position " << po->_position
        << ", velocity " << po->_velocity << " (abs " << po->_velocity.length() << ")"
        << endl;
  }
}

void setupUniverse() {
  if(!loadSettings(universe)) {
    cout << "Failed to load settings\n";
    exit(1);
  }
  if(!loadPhysicalObjects(universe)) {
    cout << "Failed to load physical objects\n";
    exit(1);
  }

  universe.getSnapshot(snapshot);
  cout << "Initial state of objects:\nCount: " << snapshot._objects.size() << endl;
  printPhysicalObjects(snapshot._objects);
}

void setupOpenGL(int & argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("Ksztalt");
  glutDisplayFunc(paint);
  glutReshapeFunc(reshape);
//  glutKeyboardFunc(klawiaturka);
}

void setProjectionData() {
  double physicalObjectExtremum = 0.0;
  for(const auto & po : snapshot._objects) {
    const auto & pos = po->_position;
    if(fabs(pos.v[0]) > physicalObjectExtremum) {
      physicalObjectExtremum = fabs(pos.v[0]);
    }
    if(fabs(pos.v[1]) > physicalObjectExtremum) {
      physicalObjectExtremum = fabs(pos.v[1]);
    }
  }

  double newViewBorder = physicalObjectExtremum * (1.0 + viewBorderMargin);
  if(newViewBorder > currentViewBorder) {
    currentViewBorder = newViewBorder;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // ustaw macierz rzutowania jako jednostkową
  glOrtho(-currentViewBorder, currentViewBorder, -currentViewBorder, currentViewBorder,
          -0.5, 0.5);
}

void paint() {
  setProjectionData();

  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(lineWidthPix);

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-initialViewBorder, -initialViewBorder, 0);
  glVertex3f(initialViewBorder, -initialViewBorder, 0);
  glVertex3f(initialViewBorder, -initialViewBorder, 0);
  glVertex3f(initialViewBorder, initialViewBorder, 0);
  glVertex3f(initialViewBorder, initialViewBorder, 0);
  glVertex3f(-initialViewBorder, initialViewBorder, 0);
  glVertex3f(-initialViewBorder, initialViewBorder, 0);
  glVertex3f(-initialViewBorder, -initialViewBorder, 0);
  glEnd();

  paintPhysicalObjects();

  glFlush();
  glutSwapBuffers();
}

void reshape(int, int) {
//  glViewport(0, 0, 800, 800);
  paint();
}

void readUniverse(int) {
  if(!doPaintPhysicalObejcts) {
    return;
  }

//  cout << "readUniverse()\n";
  glutTimerFunc(repaintPeriodMs, readUniverse, 0);

  universe.getSnapshot(snapshot);
  if(snapshot._collisionDetected) {
    cout << "collision!\n";
    doPaintPhysicalObejcts = false;
  }

//  cout << "current tick: " << snapshot._currentTick << endl;
//  printPhysicalObjects(snapshot._objects);
//  if(snapshot._currentTick >= 10000) {
//    doPaintPhysicalObejcts = false;
//  }

//  glutPostRedisplay();
  paint();
}

void paintPhysicalObjects() {
  const double physObjDrawSize = currentViewBorder * physicalObjectSize;

  int i = 0;
  for(const auto & po : snapshot._objects) {
    glColor3fv(getColorForPhysicalObject(i));

    const Vector & pos = po->_position;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const double radius = static_cast<const SphericalObject &>(*po)._radius;
      drawFilledCircle(pos.v[0], pos.v[1], radius, 30);
    }
    else {
      glBegin(GL_LINES);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] + physObjDrawSize, 0);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] - physObjDrawSize, 0);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] + physObjDrawSize, 0);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] - physObjDrawSize, 0);
      glEnd();
    }

    ++i;
  }
}

const GLfloat * getColorForPhysicalObject(int index) {
  int i = index % (sizeof(physObjColor) / sizeof(physObjColor[0]));
  return physObjColor[i];
}
