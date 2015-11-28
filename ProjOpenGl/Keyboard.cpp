#include <iostream>
#include "MainData.h"

using namespace std;

void handleKeyPressed(unsigned char key, int x, int y) {
  cout << "pressed: " << (int)key << " (" << key
       << "), mouse: " << x << ", " << y << std::endl;

  switch(key) {
  case 'p': {
      auto newProj = camera.getProjection() == Camera::Projection::Orto ?
          Camera::Projection::Frustum : Camera::Projection::Orto;
      camera.setProjection(newProj);
      cout << "Projection set to "
           << (newProj == Camera::Projection::Orto ? "orto" : "frustum") << endl;
    }
    break;

  case '[':
    camera.setFollowAllObjects(!camera.getFollowAllObjects());
    break;

  // translation
  case 'w':
    camera.translate(Camera::Axis::Z, 0.1);
    break;
  case 's':
    camera.translate(Camera::Axis::Z, -0.1);
    break;
  case 'd':
    camera.translate(Camera::Axis::X, -0.1);
    break;
  case 'a':
    camera.translate(Camera::Axis::X, 0.1);
    break;
  case 'r':
    camera.translate(Camera::Axis::Y, -0.1);
    break;
  case 'f':
    camera.translate(Camera::Axis::Y, 0.1);
    break;

  // rotation
  case 'j':
    camera.rotate(Camera::Axis::Y, -10);
    break;
  case 'l':
    camera.rotate(Camera::Axis::Y, 10);
    break;
  case 'i':
    camera.rotate(Camera::Axis::X, -10);
    break;
  case 'k':
    camera.rotate(Camera::Axis::X, 10);
    break;
  case 'u':
    camera.rotate(Camera::Axis::Z, -10);
    break;
  case 'o':
    camera.rotate(Camera::Axis::Z, 10);
    break;

  // frustum parameters
  case 'x': // zoom in
    camera.changeProjectionPlaneSize(-0.1);
    break;
  case 'z': // zoom out
    camera.changeProjectionPlaneSize(0.1);
    break;
  case 'v':
    camera.changeFrustumFar(0.2);
    break;
  case 'c':
    camera.changeFrustumFar(-0.2);
    break;

  // light options
  case 'm':
    lightFromCamera = !lightFromCamera;
    break;
  case 'n':
    directionalLight = !directionalLight;
    break;

  default:
    break;
  }
}



