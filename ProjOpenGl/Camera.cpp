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
{
}

void Camera::setFollowAllObjects(bool f) {
  if(!_followAllObjects && f) {
    _ortoParams._valid = false;
    _frustumParams._valid = false;
  }
  _followAllObjects = f;
}

void Camera::setProjection(Projection p) {
  _projection = p;
  _followAllObjects = true;
  _updateGlProjection = true;
  _extremeCoordinates._valid = false;
  _ortoParams._valid = false;
  _frustumParams._valid = false;
}

void Camera::updateView(const Universe::Snapshot & s) {
  _currObjects = &s._objects;

  if(_followAllObjects || !projectionParametersValid()) {
    readExtremeCoordinates();
    setOptimalPositionForCamera();
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

  if(someObjectsFound) {
    _extremeCoordinates._valid = true;
  }
  else {
    _extremeCoordinates._valid = false;
    _extremeCoordinates._coord.fill(0);
  }
}

void Camera::setOptimalPositionForCamera() {
  if(!_extremeCoordinates._valid) {
    return;
  }

  glm::dvec3 translVect(0);

  // Move camera backward to see all objects or move forward to get closer.
  double zMargin = (_extremeCoordinates._coord[5] - _extremeCoordinates._coord[4])
                   * _optimalCameraZPositionFactor;
  translVect.z = -(_extremeCoordinates._coord[5] + zMargin);

  if(projectionParametersValid()) {
    // When proj parameters are valid only moving camera back is allowed.
    if(translVect.z < 0.0) {
      translateWorld(translVect);
    }
    return;
  }

  // center in x and y ranges
  double diff = _extremeCoordinates._coord[1] - _extremeCoordinates._coord[0];
  translVect.x = -(_extremeCoordinates._coord[1] - diff / 2.0);
  diff = _extremeCoordinates._coord[3] - _extremeCoordinates._coord[2];
  translVect.y = -(_extremeCoordinates._coord[3] - diff / 2.0);

  translateWorld(translVect);
}

void Camera::translateWorld(const glm::dvec3 & tv) {
//  std::cout << "translateWorld() vect: " << tv.x << ", " << tv.y << ", " << tv.z << "\n";

  storeModelViewMatrixAndLoad1();
  glTranslated(tv.x, tv.y, tv.z);
  multiplyByStoredModelViewMatrix();

  readExtremeCoordinates(); // TODO: do clculations on extr. coord. instead of iterating all objects
}

void Camera::rotateWorld(double angleDeg, const glm::dvec3 & rv) {
  storeModelViewMatrixAndLoad1();
  glRotated(angleDeg, rv.x, rv.y, rv.z);
  multiplyByStoredModelViewMatrix();

  readExtremeCoordinates(); // TODO: do clculations on extr. coord. instead of iterating all objects
}

void Camera::calculateProjectionParameters() {
  if(!_extremeCoordinates._valid) {
    return;
  }

  if(_projection == Projection::Orto) {
    auto newParams = getOrtoParamsToSeeAll();
    if(!_ortoParams._valid) {
      _ortoParams = newParams;
    }
    else if(_followAllObjects) {
      if(newParams._right > _ortoParams._right) {
        _ortoParams._right = newParams._right;
      }
      if(newParams._left < _ortoParams._left) {
        _ortoParams._left = newParams._left;
      }
      if(newParams._top > _ortoParams._top) {
        _ortoParams._top = newParams._top;
      }
      if(newParams._bottom < _ortoParams._bottom) {
        _ortoParams._bottom = newParams._bottom;
      }
      if(newParams._far > _ortoParams._far) {
        _ortoParams._far = newParams._far;
      }
      if(newParams._near < _ortoParams._near) {
        _ortoParams._near = newParams._near;
      }
    }

  }
  else {
    auto newParams = getFrustumParamsToSeeAllInFrontOfCamera();
    if(!_frustumParams._valid) {
      _frustumParams = newParams;
    }
    else if(_followAllObjects) {
      // Frustum is always symmetrical.
      double currAngleX = atan(_frustumParams._right / _frustumParams._near);
      double currAngleY = atan(_frustumParams._top / _frustumParams._near);
      double newAngleX = atan(newParams._right / newParams._near);
      double newAngleY = atan(newParams._top / newParams._near);
      double nextAngleX = std::max(currAngleX, newAngleX);
      double nextAngleY = std::max(currAngleY, newAngleY);
      double nextNear = std::min(_frustumParams._near, newParams._near);
      double nextFar = std::max(_frustumParams._far, newParams._far);

      _frustumParams._right = tan(nextAngleX) * nextNear;
      _frustumParams._left = -_frustumParams._right;
      _frustumParams._top = tan(nextAngleY) * nextNear;
      _frustumParams._bottom = -_frustumParams._top;
      _frustumParams._near = nextNear;
      _frustumParams._far = nextFar;
    }
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(_frustumParams._left, _frustumParams._right,
              _frustumParams._bottom, _frustumParams._top,
              _frustumParams._near, _frustumParams._far);
  }
}

std::pair<double, double> Camera::getProjectionPlaneSize() const {
  const ProjParams & pp = _projection == Projection::Orto ? _ortoParams : _frustumParams;
  if(pp._valid) {
    return { pp._right - pp._left, pp._top - pp._bottom };
  }
  else {
    return { 0, 0 };
  }
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

void Camera::changeFrustumNear(double factor) {
  if(_projection != Projection::Frustum || !_frustumParams._valid || !_extremeCoordinates._valid) {
    return;
  }

  double value = (_extremeCoordinates._coord[5] - _extremeCoordinates._coord[4]) * factor;
  if(fabs(value) < 1.0) {
    value = factor > 0.0 ? 1.0 : -1.0;
  }

  _frustumParams._near += value;
  if(_frustumParams._near < _frustumNearMin) {
    _frustumParams._near = _frustumNearMin;
  }
  if(_frustumParams._near >= _frustumParams._far) {
    _frustumParams._far = _frustumParams._near + std::abs(value);
  }

  std::cout << "changeFrustumNear(): value " << value
      << ", near " << _frustumParams._near << ", far " << _frustumParams._far
      << ", ext near: " << _extremeCoordinates._coord[5]
      << ", ext far: " << _extremeCoordinates._coord[4]
      << std::endl;

  _updateGlProjection = true;
  _followAllObjects = false;
}

void Camera::changeFrustumFar(double factor) {
  if(_projection != Projection::Frustum || !_frustumParams._valid || !_extremeCoordinates._valid) {
    return;
  }

  double value = (_extremeCoordinates._coord[5] - _extremeCoordinates._coord[4]) * factor;
  if(fabs(value) < 1.0) {
    value = factor > 0.0 ? 1.0 : -1.0;
  }

  _frustumParams._far += value;
  if(_frustumParams._far <= _frustumParams._near) {
    _frustumParams._far = _frustumParams._near + 1;
  }

  std::cout << "changeFrustumFar(): value " << value
      << ", frustum near " << _frustumParams._near << ", frustum far " << _frustumParams._far
      << ", ext near: " << _extremeCoordinates._coord[5]
      << ", ext far: " << _extremeCoordinates._coord[4]
      << std::endl;

  _updateGlProjection = true;
  _followAllObjects = false;
}

void Camera::changeProjectionPlaneSize(double factor) {
  ProjParams & projParams = _projection == Projection::Orto ? _ortoParams : _frustumParams;
  ProjParams & refParams = _referenceForChangingProjectionPlaneSize; // make name shorter
  if(_followAllObjects || !refParams._valid) {
    refParams = projParams;
  }

  if(!projParams._valid || !refParams._valid) {
    return;
  }

  double refX = refParams._right - refParams._left;
  double refY = refParams._top - refParams._bottom;
  double changeX, changeY;

  bool changed = false; // for message
  for(int i=0; i<10; ++i) {
    changeX = refX * factor;
    changeY = refY * factor;
    ProjParams newProjParams = projParams;

    newProjParams._right += changeX;
    newProjParams._left -= changeX;
    newProjParams._top += changeY;
    newProjParams._bottom -= changeY;

    if(newProjParams._right <= newProjParams._left ||
       newProjParams._top <= newProjParams._bottom) {
      factor /= 10.0;
    }
    else {
      projParams = newProjParams;
      changed = true;
      break;
    }
  }

  std::cout << "changeProjectionPlaneSize(): ";
  if(changed) {
    std::cout << "factor: " << factor << ", changeX: " << changeX << ", changeY: " << changeY;
  } else {
    std::cout << "nothing changed";
  }
  std::cout << std::endl;

  _updateGlProjection = true;
  _followAllObjects = false;

  // TODO: Remember zoom history. Then zoom in-out will work better at small factors.
}

void Camera::translate(Axis a, double factor) {
  glm::dvec3 tv(0);
  auto projPlaneSize = getProjectionPlaneSize();
  const double refValue = (projPlaneSize.first + projPlaneSize.second) / 2;

  switch(a) {
  case Axis::X:
    tv.x = refValue * factor;
    break;
  case Axis::Y:
    tv.y = refValue * factor;
    break;
  case Axis::Z:
    tv.z = refValue * factor;
    break;
  }

  std::cout << "translate(): " << tv.x << ", " << tv.y << ", " << tv.z << std::endl;

  translateWorld(tv);
  _followAllObjects = false;
}

void Camera::rotate(Axis a, double angleDeg) {
  glm::dvec3 rv(0);

  switch(a) {
  case Axis::X:
    rv.x = 1;
    break;
  case Axis::Y:
    rv.y = 1;
    break;
  case Axis::Z:
    rv.z = 1;
    break;
  }

  rotateWorld(angleDeg, rv);
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

void Camera::loadModelViewMatrixToCache() {
  glGetDoublev(GL_MODELVIEW_MATRIX, &_cachedModelViewMatrix[0][0]);
}

glm::dvec4 Camera::cachedModelViewMatrix_Mul_Vector(const Vector & v) {
  glm::dvec4 point = { v.v[0], v.v[1], v.v[2], 1 };
  glm::dvec4 ret = _cachedModelViewMatrix * point;
  return ret;
}
