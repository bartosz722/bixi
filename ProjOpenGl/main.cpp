#include "main.h"

using namespace std;

const int windowWidth = 800;
const int windowHeight = 800;
const int repaintPeriodMs = 20;
const double initialViewBorder = 31850000;
const double physicalObjectSize = 0.03; // % of wiew width
const int lineWidthPix = 3;
const int lineWidthTrack = 1;
const double viewBorderMargin = 0.03; // % of extreme coordinate
const size_t trackLength = 1000;
const size_t trackDensity = 5;

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

bool doPaintPhysicalObejcts = true;
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
  if(!loadSettings(universe)) {
    cout << "Failed to load settings\n";
    exit(1);
  }
  if(!loadPhysicalObjects(universe)) {
    cout << "Failed to load physical objects\n";
    exit(1);
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
  paintTracks();

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

  tracker.pushData(snapshot._objects);

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
  static int ti=0;
  ++ti;

  const auto & tracks = tracker.getTracks();
  for(const auto & t : tracks) {
    if(ti % 100 == 0) {
      cout << "track of " << t.first << " size " << t.second.size() << endl;
    }

    glColor3ubv(objData[t.first]._trackColor.rgbData());
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
