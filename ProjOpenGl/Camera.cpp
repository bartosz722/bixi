#include <iostream>
#include <algorithm>
#include <cmath>
#include <GL/gl.h>
#include "Camera.h"
#include "SphericalObject.h"
#include "Assert.h"

Camera::Camera()
  : _followAllObjects(true)
  , _projection(Projection::Orto)
  , _updateGlProjection(true)
  , _currObjects(NULL)
  , _projectionPlaneSize(0,0)
{
}

void Camera::setFollowAllObjects(bool f) {
  _followAllObjects = f;
}

void Camera::setProjection(Projection p) {
  _projection = p;
  _updateGlProjection = true;
  _extremeCoordinates._valid = false;
  _ortoParams._valid = false;
  _frustumParams._valid = false;
}

// jesli orto to:
// - cofaj kamere jeśli obiekt zbliżył się do kamery - nie trzeba w tym rzutowaniu
// - centruj kamere tylko jeśli poszerzane jest również pole widzenia

// jeśli frustum to:
// - cofaj kamere jeśli obiekt zbliżył się do kamery, parametry frustum są resetowane wtedy
// - centrum się nie zmienia, pole widzenia może być tylko poszerzane i tylko symetrycznie

void Camera::updateView(const Universe::Snapshot & s) {
  _currObjects = &s._objects;

  if(_followAllObjects || !projectionParametersValid()) {
    readExtremeCoordinates();
    setOptimalPositionForCamera(); // may use current projection parameters
    calculateProjectionParameters();
    _updateGlProjection = true;
  }

  if(_updateGlProjection) {
    useProjectionParameters();
  }

  _updateGlProjection = false;
}

bool Camera::projectionParametersValid() {
  if(_projection == Projection::Orto) {
    return _ortoParams._valid;
  }
  else {
    return _frustumParams._valid;
  }
}

void Camera::readExtremeCoordinates() {
  _extremeCoordinates._valid = false;
  bool someObjectsFound = false;

  loadModelViewMatrixToCache();

  // Find extreme coordinates for current states of objects
  for(auto & po : *_currObjects) {
    if(!po->_active) {
      continue;
    }

    const auto tp = cachedModelViewMatrix_Mul_Vector(po->_position);

    double radius = 0.0;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const SphericalObject & so = static_cast<const SphericalObject &>(*po);
      radius = so._radius;
    }

    if(!someObjectsFound) {
      for(int i=0; i<3; ++i) {
        _extremeCoordinates._coord[2*i] = _extremeCoordinates._coord[2*i+1] = tp[i];
      }
      someObjectsFound = true;
    }

    for(int i=0; i<3; ++i) {
      double & extLeft = _extremeCoordinates._coord[2*i];
      double & extRight = _extremeCoordinates._coord[2*i+1];
      double left = tp[i] - radius;
      double right = tp[i] + radius;
      if(left < extLeft) {
        extLeft = left;
      }
      if(right > extRight) {
        extRight = right;
      }
    }
  }

  if(!someObjectsFound) {
    _extremeCoordinates._coord.fill(0);
  }
  else {
    // TODO: move margin calculation to calculateProjectionParameters()
    // Add margin for found extreme coordinates
    for(int i=0; i<3; ++i) {
      double diff = _extremeCoordinates._coord[2*i+1] - _extremeCoordinates._coord[2*i];
      if(diff < 0.001) {
        diff = 1.0;
      }
      _extremeCoordinates._coord[2*i] -= diff * _extremeCoordinatesMargin;
      _extremeCoordinates._coord[2*i+1] += diff * _extremeCoordinatesMargin;
    }
    _extremeCoordinates._valid = true;
  }
}

void Camera::setOptimalPositionForCamera() {
  if(!_extremeCoordinates._valid) {
    return;
  }

  glm::dvec3 translVect(0);

  // move back to see all objects
  if(_extremeCoordinates._coord[5] > 0) {
    translVect.z = -_extremeCoordinates._coord[5];
  }

  // center in x and y ranges
  double diff = _extremeCoordinates._coord[1] - _extremeCoordinates._coord[0];
  translVect.x = -(_extremeCoordinates._coord[1] - diff / 2.0);
  diff = _extremeCoordinates._coord[3] - _extremeCoordinates._coord[2];
  translVect.y = -(_extremeCoordinates._coord[3] - diff / 2.0);

  translateWorld(translVect); // with extreme coordinates
}

void Camera::translateWorld(const glm::dvec3 & tv) {
//  std::cout << "translateWorld() vect: " << tv._x << ", " << tv._y << ", " << tv._z << "\n";

  storeModelViewMatrixAndLoad1();
  glTranslated(tv.x, tv.y, tv.z);
  multiplyByStoredModelViewMatrix();

  readExtremeCoordinates(); // TODO: do clculations on extr. coord. instead of iterating all objects
}

void Camera::calculateProjectionParameters() {
  if(!_extremeCoordinates._valid) {
    return;
  }

  if(_projection == Projection::Orto) {
    _ortoParams = getOrtoParamsToSeeAll();
//    std::cout << "orto params: " << _ortoParams.toString() << std::endl;
  }
  else {
    _frustumParams = getFrustumParamsToSeeAllInFrontOfCamera();
//    std::cout << "frustum params: " << _frustumParams.toString() << std::endl;
  }
}

Camera::ProjParams Camera::getOrtoParamsToSeeAll() {
  // TODO: add adding margins here after moving it away from readExtremeCoordinates()

  ProjParams ret;
  ret._left = _extremeCoordinates._coord[0];
  ret._right = _extremeCoordinates._coord[1];
  ret._bottom = _extremeCoordinates._coord[2];
  ret._top = _extremeCoordinates._coord[3];
  ret._far = -_extremeCoordinates._coord[4];
  ret._near = -_extremeCoordinates._coord[5];
  ret._valid = true;
  return ret;
}

Camera::ProjParams Camera::getFrustumParamsToSeeAllInFrontOfCamera() {
  ProjParams ret;
  bool someObjectsFound = false;
  double maxGammaX = 0;
  double maxGammaY = 0;

  loadModelViewMatrixToCache();

  for(auto & po : *_currObjects) {
    if(!po->_active) {
      continue;
    }

    const auto tp = cachedModelViewMatrix_Mul_Vector(po->_position);

    double radius = 0;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const SphericalObject & so = static_cast<const SphericalObject &>(*po);
      radius = so._radius;
    }
    double minusZMinusRadius = -tp.z - radius;
    double minusZPlusRadius = -tp.z + radius;

    if(minusZMinusRadius < _frustumNearMin) {
      continue;
    }

    if(!someObjectsFound) {
      ret._near = minusZMinusRadius;
      ret._far = minusZPlusRadius;
      someObjectsFound = true;
    }

    if(minusZMinusRadius < ret._near) {
      ret._near = minusZMinusRadius;
    }
    if(minusZPlusRadius > ret._far) {
      ret._far = minusZPlusRadius;
    }

    // length of position vector projected on XZ or YZ plane
    double kx = sqrt(pow(tp.x, 2) + pow(tp.z, 2));
    double ky = sqrt(pow(tp.y, 2) + pow(tp.z, 2));

    // angle between position vector and Z axis
    double alphaX = atan(tp.x / -tp.z);
    double alphaY = atan(tp.y / -tp.z);

    // angle between position vector and sphere's tangent line
    double betaX = asin(radius / kx);
    double betaY = asin(radius / ky);

    // angle between sphere's tangent line and Z axis
    double gammaX = std::max(fabs(alphaX + betaX), fabs(alphaX - betaX));
    double gammaY = std::max(fabs(alphaY + betaY), fabs(alphaY - betaY));

//    std::cout << "kx: " << kx << ", alphaX: " << alphaX << ", betaX: " << betaX
//        << ", gammaX: " << gammaX
//        << ", abs+: " << fabs(alphaX + betaX) << ", abs-: " << fabs(alphaX - betaX)
//        << std::endl;

    if(gammaX > maxGammaX) {
      maxGammaX = gammaX;
    }
    if(gammaY > maxGammaY) {
      maxGammaY = gammaY;
    }

    // TODO: margins!
  }

  if(someObjectsFound) {
    // Frustum is always symmetrical.
    double aX = tan(maxGammaX);
    double aY = tan(maxGammaY);
    ret._right = aX * ret._near;
    ret._left = -ret._right;
    ret._top = aY * ret._near;
    ret._bottom = -ret._top;
    ret._valid = true;

//    std::cout << "frustum: aX: " << aX << ", aY: " << aY << std::endl;
  }

  return ret;
}

void Camera::useProjectionParameters() {
  if(_projection == Projection::Orto) {
    if(!_ortoParams._valid) {
      return;
    }

    _projectionPlaneSize.first = _ortoParams._right - _ortoParams._left;
    _projectionPlaneSize.second = _ortoParams._top - _ortoParams._bottom;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(_ortoParams._left, _ortoParams._right,
            _ortoParams._bottom, _ortoParams._top,
            _ortoParams._near, _ortoParams._far);
  }
  else {
    if(!_frustumParams._valid) {
      return;
    }

    _projectionPlaneSize.first = _frustumParams._right - _frustumParams._left;
    _projectionPlaneSize.second = _frustumParams._top - _frustumParams._bottom;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(_frustumParams._left, _frustumParams._right,
              _frustumParams._bottom, _frustumParams._top,
              _frustumParams._near, _frustumParams._far);
  }
}

std::pair<double, double> Camera::getProjectionPlaneSize() const {
  return _projectionPlaneSize;
}

double Camera::getGreatestCoordinateDifference() const {
  double ret = 0.0;
  if(_extremeCoordinates._valid) {
    for(int i=0; i<3; ++i) {
      double d = _extremeCoordinates._coord[2*i+1] - _extremeCoordinates._coord[2*i];
      if(d > ret) {
        ret = d;
      }
    }
  }
  return ret;
}

void Camera::addToFrustumNear(double factor) {
  if(_projection != Projection::Frustum || !_frustumParams._valid || !_extremeCoordinates._valid) {
    return;
  }

  double value = (_extremeCoordinates._coord[4] - _extremeCoordinates._coord[5]) * factor;
  if(fabs(value) < 1.0) {
    value = factor > 0.0 ? 1.0 : -1.0;
  }

  _frustumParams._near += value;
  if(_frustumParams._near < _frustumNearMin) {
    _frustumParams._near = _frustumNearMin;
  }
  if(_frustumParams._near > _frustumParams._far) {
    _frustumParams._far = _frustumParams._near + std::abs(value);
  }

  std::cout << "addToFrustumNear(): value " << value << ", near " << _frustumParams._near
      << ", far " << _frustumParams._far << std::endl;

  _updateGlProjection = true;
  _followAllObjects = false;
}

void Camera::translate(Axis a, double factor) {
  double tx = 0, ty = 0, tz = 0;
  const double refValue = (_projectionPlaneSize.first + _projectionPlaneSize.second) / 2;

  switch(a) {
  case Axis::X:
    tx = refValue * factor;
    break;
  case Axis::Y:
    ty = refValue * factor;
    break;
  case Axis::Z:
    tz = refValue * factor;
    break;
  }

  std::cout << "translate(): " << tx << ", " << ty << ", " << tz << std::endl;

  storeModelViewMatrixAndLoad1();
  glTranslated(tx, ty, tz);
  multiplyByStoredModelViewMatrix();

  _followAllObjects = false;
  readExtremeCoordinates(); // TODO: do clculations on extr. coord. instead of iterating all objects
}

void Camera::rotate(Axis a, double angleDeg) {
  double rx = 0, ry = 0, rz = 0;

  switch(a) {
  case Axis::X:
    rx = 1;
    break;
  case Axis::Y:
    ry = 1;
    break;
  case Axis::Z:
    rz = 1;
    break;
  }

  storeModelViewMatrixAndLoad1();
  glRotated(angleDeg, rx, ry, rz);
  multiplyByStoredModelViewMatrix();

  _followAllObjects = false;
  readExtremeCoordinates(); // TODO: do clculations on extr. coord. instead of iterating all objects
}

void Camera::storeModelViewMatrixAndLoad1() {
  glMatrixMode(GL_MODELVIEW);
  glGetDoublev(GL_MODELVIEW_MATRIX, _storedModelViewMatrix);
  glLoadIdentity();
}

void Camera::multiplyByStoredModelViewMatrix() {
  glMultMatrixd(_storedModelViewMatrix);
}

void Camera::loadModelViewMatrixToCache() {
  glGetDoublev(GL_MODELVIEW_MATRIX, &_cachedModelViewMatrix[0][0]);
}

glm::dvec4 Camera::cachedModelViewMatrix_Mul_Vector(const Vector & v) {
  glm::dvec4 point = { v.v[0], v.v[1], v.v[2], 1 };
  glm::dvec4 ret = _cachedModelViewMatrix * point;
  return ret;
}
