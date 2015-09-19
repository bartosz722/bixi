#include <iostream>
#include <algorithm>
#include <cmath>
#include <GL/gl.h>
#include "Camera.h"
#include "SphericalObject.h"

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

  if(_followAllObjects) {
    // temporary actions
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
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
    setOptimalPositionForCamera(); // may interact with proj parameters
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

  // Find extreme coordinates for current states of objects
  for(auto & po : *_currObjects) {
    if(!po->_active) {
      continue;
    }

    // tp - position translated by model-view matrix
    // TODO: don't assume this matrix is identity
    const Vector tp = po->_position;

    double radius = 0.0;
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const SphericalObject & so = static_cast<const SphericalObject &>(*po);
      radius = so._radius;
    }

    if(!someObjectsFound) {
      for(int i=0; i<3; ++i) {
        _extremeCoordinates._coord[2*i] = _extremeCoordinates._coord[2*i+1] = tp.v[i];
      }
      someObjectsFound = true;
    }

    for(int i=0; i<3; ++i) {
      double & extLeft = _extremeCoordinates._coord[2*i];
      double & extRight = _extremeCoordinates._coord[2*i+1];
      double left = tp.v[i] - radius;
      double right = tp.v[i] + radius;
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

  GlVector translVect;

  // move back to see all objects
  if(_extremeCoordinates._coord[5] > 0) {
    translVect._z = -_extremeCoordinates._coord[5];
  }

  // center in x and y ranges
  double diff = _extremeCoordinates._coord[1] - _extremeCoordinates._coord[0];
  translVect._x = -(_extremeCoordinates._coord[1] - diff / 2.0);
  diff = _extremeCoordinates._coord[3] - _extremeCoordinates._coord[2];
  translVect._y = -(_extremeCoordinates._coord[3] - diff / 2.0);

  translateWorld(translVect); // with extreme coordinates
}

void Camera::translateWorld(GlVector tv) {
  std::cout << "translateWorld() vect: " << tv._x << ", " << tv._y << ", " << tv._z << "\n";

  storeModelViewMatrixAndLoad1();
  glTranslated(tv._x, tv._y, tv._z);
  multiplyByStoredModelViewMatrix();

  readExtremeCoordinates(); // do clculations on extr. coord. instead of iterating all objects
}

void Camera::calculateProjectionParameters() {
  if(!_extremeCoordinates._valid) {
    return;
  }

  if(_projection == Projection::Orto) {
    _ortoParams = getOrtoParamsToSeeAll();
    std::cout << "orto params: " << _ortoParams.toString() << std::endl;
  }
  else {
    _frustumParams = getFrustumParamsToSeeAllInFrontOfCamera();
    std::cout << "frustum params: " << _frustumParams.toString() << std::endl;
  }
}

Camera::ProjParams Camera::getOrtoParamsToSeeAll() {
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
  double aX = 0, aY = 0;
  bool someObjectsFound = false;

  for(auto & po : *_currObjects) {
    if(!po->_active) {
      continue;
    }

    // TODO: margins!

    // tp - position translated by model-view matrix
    // TODO: don't assume this matrix is identity
    const Vector tp = po->_position;

    double radius = 1; // set small radius for objects with no radius
    if(po->getType() == PhysicalObjectType::SphericalObject) {
      const SphericalObject & so = static_cast<const SphericalObject &>(*po);
      radius = so._radius;
    }
    double minusZMinusRadius = -tp.v[2] - radius;
    double minusZPlusRadius = -tp.v[2] + radius;

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

    // div by zero won't occur
    double currAX = (std::abs(tp.v[0]) + radius) / std::abs(tp.v[2]);
    double currAY = (std::abs(tp.v[1]) + radius) / std::abs(tp.v[2]);

    if(currAX > aX) {
      aX = currAX;
    }
    if(currAY > aY) {
      aY = currAY;
    }
  }

  if(someObjectsFound) {
    // Frustum is always symmetrical.
    ret._right = aX * ret._near;
    ret._left = -ret._right;
    ret._top = aY * ret._near;
    ret._bottom = -ret._top;
    ret._valid = true;
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
    glOrtho(_frustumParams._left, _frustumParams._right,
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

void Camera::addToFrustumNear(double value) {
  if(_projection != Projection::Frustum || !_frustumParams._valid) {
    return;
  }

  _frustumParams._near += value;
  if(_frustumParams._near < _frustumNearMin) {
    _frustumParams._near = _frustumNearMin;
  }
  if(_frustumParams._near > _frustumParams._far) {
    _frustumParams._far = _frustumParams._near + std::abs(value);
  }

  _updateGlProjection = true;
  _followAllObjects = false;
}

void Camera::translate(Axis a, double factor) {
  if(!_extremeCoordinates._valid) {
    return;
  }

  double tx = 0, ty = 0, tz = 0;
  double diff = 0;

  switch(a) {
  case Axis::X:
    // TODO: some other reference value
    diff = _extremeCoordinates._coord[1] - _extremeCoordinates._coord[0];
    tx = diff * factor;
    break;
  case Axis::Y:
    diff = _extremeCoordinates._coord[3] - _extremeCoordinates._coord[2];
    ty = diff * factor;
    break;
  case Axis::Z:
    diff = _extremeCoordinates._coord[5] - _extremeCoordinates._coord[4];
    tz = diff * factor;
    break;
  }

  storeModelViewMatrixAndLoad1();
  glTranslated(tx, ty, tz);
  multiplyByStoredModelViewMatrix();

  _followAllObjects = false;
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
}

void Camera::storeModelViewMatrixAndLoad1() {
  glMatrixMode(GL_MODELVIEW);
  glGetDoublev(GL_MODELVIEW_MATRIX, _storedModelViewMatrix);
  glLoadIdentity();
}

void Camera::multiplyByStoredModelViewMatrix() {
  glMultMatrixd(_storedModelViewMatrix);
}
