#include <iostream>
#include "MainData.h"

void handleKeyPressed(unsigned char key, int x, int y) {
  std::cout << "pressed: " << (int)key << " (" << key
            << "), mouse: " << x << ", " << y << std::endl;

  switch(key) {
  case 'p':
    camera.setProjection(
        camera.getProjection() == Camera::Projection::Orto ?
        Camera::Projection::Frustum : Camera::Projection::Orto);
    break;

  case '[':
    camera.setFollowAllObjects(!camera.getFollowAllObjects());
    break;

  case 'x': // zoom in
    camera.addToFrustumNear(0.3);
    break;

  case 'z': // zoom out
    camera.addToFrustumNear(-0.3);
    break;

  default:
    break;
  }
}



