#include "main.h"

using namespace std;

const int windowWidth = 800;
const int windowHeight = 800;
const int repaintPeriodMs = 20;
const int printPhysObjectsPeriod = 50 * 3; // unit: one readUniverse() call
const size_t trackLength = 1000;
const size_t trackDensity = 2;

const double physicalObjectSize = 0.02; // fraction of something
const int lineWidthPhysObj = 3;
const int lineWidthSpherObj = 2;
const int lineWidthTrack = 1;
const int lineWidthViewportBorder = 2;

const Color defaultColors[] = {
  { 0, 0, 255 },
  { 0, 0, 0 },
  { 255, 0, 0 },
};

const float light_off[] = { 0, 0, 0, 1 };

struct PhysObjData {
  Color _color;
  Texture _texture; // "real" texture, from image
  Texture _colorTexture; // one color texture
};

bool precisionTestMode = false;
bool lightsEnabled = true;
bool texturesEnabled = false;

Universe universe;
Universe::Snapshot snapshot;
Tracker tracker(trackDensity, trackLength);
map<int, PhysObjData> objData;
Texture borderColorTexture;

bool doPaintPhysicalObejcts = true;
size_t readUniverseCallCount = 0;

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
        << ": active " << po->_active
        << ", position " << po->_position
        << ", velocity " << po->_velocity << " (abs " << po->_velocity.length() << ")"
        << ", mass " << po->_mass;

    if(po->getType() == PhysicalObjectType::Spacecraft) {
      Spacecraft & s = static_cast<Spacecraft &>(*po);
      cout << ", engine on " << s._engineOn << ", prop. mass " << s._propellantMass;
    }

    cout << endl;
  }
}

void setupUniverse() {
  if(!precisionTestMode) {
    if(!loadUniverseData(universe)) {
      cout << "Failed to universe data\n";
      exit(1);
    }
  }
  else {
    loadPrecisionTestData(universe);
  }

  setupLocalPhysicalObjectData();

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
  glutKeyboardFunc(handleKeyPressed);
  glEnable(GL_DEPTH_TEST);

  if(lightsEnabled) {
    setupLights();
  }

  if(texturesEnabled) {
    setupTextures();
  }

  camera.setProjection(Camera::Projection::Orto);
  camera.setFollowAllObjects(true);
}

void setViewport() {
  auto projSize = camera.getProjectionPlaneSize();
  double ratio = projSize.second / projSize.first;
  double vx = windowWidth;
  double vy = ratio * vx;
  if(vy > windowHeight) {
    vy = windowHeight;
    vx = vy / ratio;
  }

  double offsetX = (windowWidth - vx) / 2;
  double offsetY = (windowHeight - vy) / 2;

  glViewport(offsetX, offsetY, vx, vy);
}

void paint() {
  camera.updateView(snapshot);
  setViewport();

  glClearColor(0, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(lightsEnabled) {
    setLightProperties();
  }

  paintPhysicalObjects();
  paintTracks();
  paintBorderAroundViewport();

  glFlush();
  glutSwapBuffers();
}

void reshape(int, int) {
  paint();
}

void readUniverse(int) {
  ++readUniverseCallCount;

  if(!doPaintPhysicalObejcts) {
    return;
  }

//  cout << "readUniverse()\n";
  glutTimerFunc(repaintPeriodMs, readUniverse, 0);

  universe.getSnapshot(snapshot);
  if(!snapshot._running) {
    cout << "Universe stopped.\n";
    doPaintPhysicalObejcts = false;
  }
  if(snapshot._stoppedByCollision) {
    cout << "Collision detected!\n";
    doPaintPhysicalObejcts = false;
  }

  tracker.pushData(snapshot._objects);

  if(readUniverseCallCount % printPhysObjectsPeriod == 0 || !doPaintPhysicalObejcts) {
    cout << "current tick: " << snapshot._currentTick << endl;
    printPhysicalObjects(snapshot._objects);
    cout << "size of longest track: " << tracker.getSizeOfLongestTrack() << endl;

    if(precisionTestMode) {
      printPrecisionTestResult();
    }
  }

//  glutPostRedisplay();
  paint();
}

void paintPhysicalObjects() {
  const double physObjDrawSize = camera.getGreatestCoordinateDifference() * physicalObjectSize;

  int i = 0;
  for(const auto & po : snapshot._objects) {
    if(!po->_active) {
      continue;
    }

    auto & currObjData = objData[po->getId()];

    if(!texturesEnabled) {
      glColor3ubv(currObjData._color.rgbData());
    }
    else {
      if(currObjData._texture.isValid()) {
        currObjData._texture.use();
      }
      else {
        currObjData._colorTexture.use();
      }
    }

    const Vector & pos = po->_position;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const double radius = static_cast<const SphericalObject &>(*po)._radius;
      glLineWidth(lineWidthSpherObj);
      drawGluSphere(pos.v[0], pos.v[1], pos.v[2], radius, 50, 50, texturesEnabled);
    }
    else {
      glLineWidth(lineWidthPhysObj);
      glBegin(GL_LINES);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] + physObjDrawSize, pos.v[2]);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] - physObjDrawSize, pos.v[2]);
      glVertex3f(pos.v[0] + physObjDrawSize, pos.v[1] + physObjDrawSize, pos.v[2]);
      glVertex3f(pos.v[0] - physObjDrawSize, pos.v[1] - physObjDrawSize, pos.v[2]);
      glEnd();
    }

    ++i;
  }
}

void paintTracks() {
  const auto & tracks = tracker.getTracks();
  for(const auto & t : tracks) {
    if(!texturesEnabled) {
      glColor3ubv(objData[t.first]._color.rgbData());
    }
    else {
      objData[t.first]._colorTexture.use();
    }

    glLineWidth(lineWidthTrack);
    glBegin(GL_LINE_STRIP);

    for(const auto & point : t.second) {
      glVertex3d(point.v[0], point.v[1], point.v[2]);
    }

    glEnd();
  }
}

void paintBorderAroundViewport() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if(!texturesEnabled) {
      glColor3ub(0, 0, 255);
    }
    else {
      borderColorTexture.use();
    }

    glLineWidth(lineWidthViewportBorder);
    glBegin(GL_LINE_LOOP);
    glVertex3d(-1, -1, 0);
    glVertex3d(-1, 1, 0);
    glVertex3d(1, 1, 0);
    glVertex3d(1, -1, 0);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void setupLocalPhysicalObjectData() {
  int defaultColorCount = 0;
  for(const auto & prop : universe.getPhysicalObjectsProperties()) {
    const int objId = prop.getId();
    if(prop._color.isValid()) {
      objData[objId]._color = prop._color;
    }
    else {
      int idx = defaultColorCount++ % (sizeof(defaultColors) / sizeof(defaultColors[0]));
      objData[objId]._color = defaultColors[idx];
    }

    if(prop._tracked) {
      tracker.trackObject(objId);
    }
  }
}

void printPrecisionTestResult() {
  PrecisionTester::Result ptr = universe.getPrecisionTestResult();
  cout << "Precision test result: orbits: " << ptr._orbitCount << endl;
  cout << "_rightXRange: " << ptr._rightXRange.first << ", "
      << ptr._rightXRange.second << endl;
  cout << "_leftXRange: " << ptr._leftXRange.first << ", "
      << ptr._leftXRange.second << endl;
  cout << "right x devi: " << ptr._rightXRangeDeviation.first << " ("
      << ptr._rightXRangeDeviationPercentage.first << " %), "
      << ptr._rightXRangeDeviation.second << " ("
      << ptr._rightXRangeDeviationPercentage.second << " %)" << endl;
  cout << "left x diff: " << ptr._leftXRangeDiff
      << " (" << ptr._leftXRangeDiffPercentage << " %)" << endl;
}

void setupLights() {
  const float light_ambient[] = { 0.6, 0.6, 0.6, 1 };
  const float light_difuse[] = { 0.4, 0.4, 0.4, 1 };
  const float light_specular[] = { 0.6, 0.6, 0.6, 1 };

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_off);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_difuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  const float material_specular[] = { 1, 1, 1, 0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128); // shininess for specular light (0-128)

  // Instead of calling glMaterialfv() for ambient and diffuse:
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  // TODO: different material and color settings for objects, tracks and border
}

void setLightProperties() {
  static const float direction[] = { 0, 0, 1, 0 };
  static const float position[] = { 0, 0, 0, 1 };

  if(lightFromCamera) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }

  glLightfv(GL_LIGHT0, GL_POSITION, directionalLight ? direction : position);

  if(lightFromCamera) {
    glPopMatrix();
  }

  // TODO: different lights for objects, tracks and border
}

void setupTextures() {
  glEnable(GL_TEXTURE_2D);

  // Replace color of object with colors from texture
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  for(const auto & prop : universe.getPhysicalObjectsProperties()) {
    const int objId = prop.getId();
    auto & currObjData = objData[objId];

    const char * textureFileName = prop._texture;
    if(textureFileName != NULL) {
      currObjData._texture.createFromImage(textureFileName);
    }

    const auto & currColor = currObjData._color;
    assert(currColor.isValid());
    currObjData._colorTexture.createOneColor(
        currColor.rgbData()[0], currColor.rgbData()[1], currColor.rgbData()[2]);
  }

  borderColorTexture.createOneColor(0, 0, 255);
}
