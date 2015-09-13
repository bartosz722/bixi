#include <iostream>
#include <algorithm>
#include <cmath>
#include <GL/gl.h>
#include "Camera.h"
#include "SphericalObject.h"

Camera::Camera()
  : _followAllObjects(true)
  , _projection(Projection::Orto)
  , _updateProjection(true)
  , _extremeCoordinatesInitialized(false)
  , _frustumNear(_frustumNearMin)
  , _frustumLeft(-1)
  , _frustumDown(-1)
  , _frustumParemetersInitialized(false)
  , _currObjects(NULL)
  , _projectionPlaneSize(0,0)
{
  _extremeCoordinates.fill(0.0);
}

void Camera::setFollowAllObjects(bool f) {
  _followAllObjects = f;
}

void Camera::setProjection(Projection p) {
  _projection = p;
  _updateProjection = true;
  _extremeCoordinatesInitialized = false;
  _frustumParemetersInitialized = false;
}

void Camera::updateView(const Universe::Snapshot & s) {
  _currObjects = &s._objects;

  if(_followAllObjects || _updateProjection || !_extremeCoordinatesInitialized) {
    updateExtremeCoordinates(_projection == Projection::Frustum);
    if(_projection == Projection::Frustum) {
      updateFrustumParameters();
    }
    _updateProjection = true;
  }

  if(!_extremeCoordinatesInitialized) {
    return;
  }

  if(_updateProjection) {
    setProjectionParameters();
  }

  _updateProjection = false;
}

bool Camera::getExtremeCoordinates(ExtremeCoordinates & result, bool useFrustumNearMin) {
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

    if(useFrustumNearMin) {
      if(tp.v[2] + radius > -_frustumNearMin) {
        continue;
      }
    }

    if(!someObjectsFound) {
      for(int i=0; i<3; ++i) {
        result[2*i] = result[2*i+1] = tp.v[i];
      }
      someObjectsFound = true;
    }

    for(int i=0; i<3; ++i) {
      double & extLeft = result[2*i];
      double & extRight = result[2*i+1];
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
    result[0] = -1; result[1] = 1;
    result[2] = -1; result[3] = 1;
    result[4] = -2; result[5] = -1;
    return false;
  }

  // Add margin for found extreme coordinates
  for(int i=0; i<3; ++i) {
    double diff = result[2*i+1] - result[2*i];
    if(diff < 0.001) {
      diff = 1.0;
    }
    result[2*i] -= diff * _extremeCoordinatesMargin;
    result[2*i+1] += diff * _extremeCoordinatesMargin;
  }

  if(useFrustumNearMin) {
    if(result[5] > -_frustumNearMin) {
      result[5] = -_frustumNearMin;
    }
  }

  return true;
}

void Camera::updateExtremeCoordinates(bool useFrustumNearMin) {
  ExtremeCoordinates newExt;
  if(!getExtremeCoordinates(newExt, useFrustumNearMin)) {
    return;
  }

  if(!_extremeCoordinatesInitialized) {
    _extremeCoordinates = newExt;
    _extremeCoordinatesInitialized = true;
  }
  else {
    // Merge with all-time extreme coordinates
    for(int i=0; i<3; ++i) {
      if(newExt[2*i] < _extremeCoordinates[2*i]) {
        _extremeCoordinates[2*i] = newExt[2*i];
      }
      if(newExt[2*i+1] > _extremeCoordinates[2*i+1]) {
        _extremeCoordinates[2*i+1] = newExt[2*i+1];
      }
    }
  }
}

void Camera::setProjectionParameters() {
  static int yy=0;

  if(_projection == Projection::Orto) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double extrXY[2];
    for(int i=0; i<2; ++i) {
      extrXY[i] = std::max(std::abs(_extremeCoordinates[2*i]), std::abs(_extremeCoordinates[2*i+1]));
    }

    _projectionPlaneSize.first = extrXY[0] * 2;
    _projectionPlaneSize.second = extrXY[1] * 2;
    glOrtho(-extrXY[0], extrXY[0],
            -extrXY[1], extrXY[1],
            -_extremeCoordinates[5], -_extremeCoordinates[4]);
  }
  else {
    double far = -_extremeCoordinates[4];
    if(far < _frustumNear) {
      far = _frustumNear + 1.0;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    _projectionPlaneSize.first = -_frustumLeft * 2;
    _projectionPlaneSize.second = -_frustumDown * 2;
    glFrustum(_frustumLeft, -_frustumLeft,
              _frustumDown, -_frustumDown,
              _frustumNear, far);

    if(yy++ % 100 == 0) {
      std::cout << "_extremeCoordinates: [4]: " << _extremeCoordinates[4]
                << ", [5]: " << _extremeCoordinates[5] << std::endl;
      std::cout << "frustum: near: " << _frustumNear << ", far: " << far << std::endl;
    }
  }
}

std::pair<double, double> Camera::getProjectionPlaneSize() const {
  return _projectionPlaneSize;
}

double Camera::getGreatestCoordinateDifference() const {
  double ret = 0.0;
  for(int i=0; i<3; ++i) {
    double d = _extremeCoordinates[2*i+1] - _extremeCoordinates[2*i];
    if(d > ret) {
      ret = d;
    }
  }
  return ret;
}

void Camera::addToFrustumNear(double value) {
  if(_projection != Projection::Frustum) {
    return;
  }

  _frustumNear += value;
  if(_frustumNear < _frustumNearMin) {
    _frustumNear = _frustumNearMin;
  }
  _updateProjection = true;
  _followAllObjects = false;
}

// TODO: merge with getExtremeCoordinates()
void Camera::updateFrustumParameters() {
  double aX = 0, aY = 0;
  bool someObjectsFound = false;

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

    if(tp.v[2] + radius > -_frustumNearMin) {
      continue;
    }

    someObjectsFound = true;

    // TODO: margins
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
    double newFrustumLeft = aX * _extremeCoordinates[5];
    double newFrustumDown = aY * _extremeCoordinates[5];
    double newFrustumNear = -_extremeCoordinates[5];
    if(!_frustumParemetersInitialized) {
      _frustumLeft = newFrustumLeft;
      _frustumDown = newFrustumDown;
      _frustumNear = newFrustumNear;
      _frustumParemetersInitialized = true;
    }
    else {
      if(newFrustumLeft < _frustumLeft) {
        _frustumLeft = newFrustumLeft;
      }
      if(newFrustumDown < _frustumDown) {
        _frustumDown = newFrustumDown;
      }
      if(newFrustumNear < _frustumNear) {
        _frustumNear = newFrustumNear;
      }
    }
  }
}
