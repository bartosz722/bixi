#include <iostream>
#include <thread>
#include <cmath>
#include <GL/glut.h>

#include "Universe.h"
#include "SphericalObject.h"

using namespace std;

void printPhysicalObjects(const PhysicalObjectsContainer & poc);
void setupUniverse();
void setupOpenGL(int & argc, char **argv);
void paint();
void paintPhysicalObjects();
void reshape(int x, int y);
void readUniverse(int value);

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
  if(!universe.loadSettings()) {
    cout << "Failed to load settings\n";
    exit(1);
  }
  if(!universe.loadPhysicalObjects()) {
    cout << "Failed to load physical objects\n";
    exit(1);
  }

  cout << "Initial state of objects:\n";
  universe.getSnapshot(snapshot);
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

  currentViewBorder = physicalObjectExtremum * (1.0 + viewBorderMargin);
  if(currentViewBorder < initialViewBorder) {
    currentViewBorder = initialViewBorder;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // ustaw macierz rzutowania jako jednostkową
  glOrtho(-currentViewBorder, currentViewBorder, -currentViewBorder, currentViewBorder, -0.5, 0.5);
}

void paint() {
  setProjectionData();

  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(lineWidthPix);

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

//  glutPostRedisplay();
  paint();
}

void paintPhysicalObjects() {
  glColor3f(0.0, 0.0, 1.0);

  const double physObjDrawSize = currentViewBorder * physicalObjectSize;

  for(const auto & po : snapshot._objects) {
    const Vector & pos = po->_position;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const double radius = static_cast<const SphericalObject &>(*po)._radius;
      const double radiusSin = sin(M_PI_4) * radius;
      glBegin(GL_LINES);
      glVertex3f(pos.v[0] - radiusSin, pos.v[1] + radiusSin, 0);
      glVertex3f(pos.v[0] + radiusSin, pos.v[1] - radiusSin, 0);
      glVertex3f(pos.v[0] + radiusSin, pos.v[1] + radiusSin, 0);
      glVertex3f(pos.v[0] - radiusSin, pos.v[1] - radiusSin, 0);
      glVertex3f(pos.v[0] + radius, pos.v[1], 0);
      glVertex3f(pos.v[0] - radius, pos.v[1], 0);
      glVertex3f(pos.v[0], pos.v[1] + radius, 0);
      glVertex3f(pos.v[0], pos.v[1] - radius, 0);
      glEnd();
    }
    else {
      glBegin(GL_LINES);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] + physObjDrawSize, 0);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] - physObjDrawSize, 0);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] + physObjDrawSize, 0);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] - physObjDrawSize, 0);
      glEnd();
    }
  }
}
