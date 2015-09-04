#include "main.h"

using namespace std;

const int windowWidth = 800;
const int windowHeight = 800;
const int repaintPeriodMs = 20;
const int printPhysObjectsPeriod = 50 * 3; // unit: one readUniverse() call
const double physicalObjectSize = 0.03; // % of wiew width
const int lineWidthPix = 3;
const int lineWidthTrack = 1;
const double viewBorderMargin = 0.2; // % of extreme coordinate
const size_t trackLength = 1000;
const size_t trackDensity = 2;

const Color defaultColors[] = {
  { 0, 0, 255 },
  { 0, 255, 0 },
  { 255, 0, 0 },
};

struct PhysObjData {
  Color _color;
  Color _trackColor;
};

Universe universe;
Universe::Snapshot snapshot;
Tracker tracker(trackDensity, trackLength);
map<int, PhysObjData> objData;

bool precisionTestMode = true;
bool doPaintPhysicalObejcts = true;
double firstViewBorder = -1;
double currentViewBorder = 0.0;
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
        << ", mass " << po->_mass
        << endl;
  }
}

void setupUniverse() {
  if(!precisionTestMode) {
    if(!loadSettings(universe)) {
      cout << "Failed to load settings\n";
      exit(1);
    }
    if(!loadPhysicalObjects(universe)) {
      cout << "Failed to load physical objects\n";
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
  if(firstViewBorder < 0.0) {
    firstViewBorder = currentViewBorder;
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

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glColor3f(1.0, 0.0, 0.0);
  glLineWidth(lineWidthPix);
  glBegin(GL_LINES);
  glVertex3f(-firstViewBorder, -firstViewBorder, 0);
  glVertex3f(firstViewBorder, -firstViewBorder, 0);
  glVertex3f(firstViewBorder, -firstViewBorder, 0);
  glVertex3f(firstViewBorder, firstViewBorder, 0);
  glVertex3f(firstViewBorder, firstViewBorder, 0);
  glVertex3f(-firstViewBorder, firstViewBorder, 0);
  glVertex3f(-firstViewBorder, firstViewBorder, 0);
  glVertex3f(-firstViewBorder, -firstViewBorder, 0);
  glEnd();

  paintPhysicalObjects();
  paintTracks();

  glFlush();
  glutSwapBuffers();
}

void reshape(int, int) {
//  glViewport(0, 0, 800, 800);
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
  const double physObjDrawSize = currentViewBorder * physicalObjectSize;

  int i = 0;
  for(const auto & po : snapshot._objects) {
    if(!po->_active) {
      continue;
    }

    glColor3ubv(objData[po->getId()]._color.rgbData());

    const Vector & pos = po->_position;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const double radius = static_cast<const SphericalObject &>(*po)._radius;
      drawFilledCircle(pos.v[0], pos.v[1], radius, 30);
    }
    else {
      glLineWidth(lineWidthPix);
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

void paintTracks() {
  const auto & tracks = tracker.getTracks();
  for(const auto & t : tracks) {
    glColor3ubv(objData[t.first]._color.rgbData());
    glLineWidth(lineWidthTrack);
    glBegin(GL_LINE_STRIP);

    for(const auto & point : t.second) {
      glVertex3d(point.v[0], point.v[1], point.v[2]);
    }

    glEnd();
  }
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
