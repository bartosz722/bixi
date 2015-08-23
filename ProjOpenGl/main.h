#ifndef PROJOPENGL_MAIN_H_
#define PROJOPENGL_MAIN_H_

#include <iostream>
#include <thread>
#include <cmath>
#include <GL/glut.h>

#include "Universe.h"
#include "SphericalObject.h"
#include "UtilGlDraw.h"
#include "DataLoader.h"
#include "Tracker.h"
#include "PhysicalObjectsContainer.h"

void printPhysicalObjects(const PhysicalObjectsContainer & poc);
void setupUniverse();
void setupOpenGL(int & argc, char **argv);
void paint();
void paintPhysicalObjects();
void paintTracks();
void reshape(int x, int y);
void readUniverse(int value);
const GLfloat * getColorForPhysicalObject(int index);

#endif /* PROJOPENGL_MAIN_H_ */
