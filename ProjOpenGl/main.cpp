#include <iostream>
#include <thread>
#include <GL/glut.h>

#include "Universe.h"

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
  glutInitWindowSize(800, 800);
  glutCreateWindow("Ksztalt");
  glutDisplayFunc(paint);
  glutReshapeFunc(reshape);
//  glutKeyboardFunc(klawiaturka);
}

void paint() {
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_LINES);

  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0, 0, 0);
  glVertex3f(1000000, 1000000, 0);

  glEnd();

  paintPhysicalObjects();

  glFlush();
  glutSwapBuffers();
}

void reshape(int, int) {
//  glViewport(0, 0, 800, 800);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // ustaw macież rzutowania jako jednostkową
  int const wii = 7500000;
  glOrtho(-wii, wii, -wii, wii, -0.5, 0.5);
  paint();
}

void readUniverse(int) {
//  cout << "readUniverse()\n";
  glutTimerFunc(20, readUniverse, 0);

  universe.getSnapshot(snapshot);
//  cout << "current tick: " << snapshot._currentTick << endl;
//  printPhysicalObjects(snapshot._objects);

//  glutPostRedisplay();
  paint();
}

void paintPhysicalObjects() {
  const int width = 8750 * 25;
  for(const auto & po : snapshot._objects) {
    const Vector & pos = po->_position;

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(pos.v[0], pos.v[1], 0);
    glVertex3f(pos.v[0] - width, pos.v[1] + width, 0);
    glEnd();
  }
}
