#ifndef PROJOPENGL_MAIN_H_
#define PROJOPENGL_MAIN_H_

#include <iostream>
#include <thread>
#include <cmath>
#include <map>
#include <GL/glut.h>

#include "Universe.h"
#include "SphericalObject.h"
#include "UtilGlDraw.h"
#include "DataLoader.h"
#include "Tracker.h"
#include "PhysicalObjectsContainer.h"
#include "MainData.h"
#include "Keyboard.h"
#include "Texture.h"

void printPhysicalObjects(const PhysicalObjectsContainer & poc);
void setupUniverse();
void setupOpenGL(int & argc, char **argv);
void setupLights();
void setupTextures();
void paint();
void paintPhysicalObjects();
void paintTracks();
void paintBorderAroundViewport();
void setLightProperties();
void reshape(int x, int y);
void readUniverse(int);
void setupLocalPhysicalObjectData();
void printPrecisionTestResult();

#endif /* PROJOPENGL_MAIN_H_ */
