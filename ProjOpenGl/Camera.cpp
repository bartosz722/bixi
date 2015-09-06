#include <iostream>
#include <GL/gl.h>
#include "Camera.h"
#include "SphericalObject.h"

Camera::Camera()
  : _followAllObjects(true)
  , _projection(Projection::Orto)
  , _updateProjection(true)
  , _firstUpdateViewCall(true)
  , _extremeCoordinatesInitialized(false)
{
  _extremeCoordinates.fill(0.0);
}

void Camera::updateView(const Universe::Snapshot & s) {
  if(_followAllObjects || _firstUpdateViewCall) {
    updateExtremeCoordinates(s);
    _updateProjection = true;
  }

  if(_updateProjection) {
    setProjectionParameters();
  }

  _updateProjection = false;
  _firstUpdateViewCall = false;
}

void Camera::updateExtremeCoordinates(const Universe::Snapshot & s) {
  if(s._objects.empty()) {
    return;
  }

  ExtremeCoordinates newExt;

  // Initialize data
  {
    const PhysicalObject & po = *s._objects[0];
    for(int i=0; i<3; ++i) {
      newExt[2*i] = newExt[2*i+1] = po._position.v[i];
    }
    if(!_extremeCoordinatesInitialized) {
      _extremeCoordinates = newExt;
      _extremeCoordinatesInitialized = true;
    }
  }

  // Find extreme coordinates for current states of objects
  for(auto & po : s._objects) {
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

    for(int i=0; i<3; ++i) {
      double & extLeft = newExt[2*i];
      double & extRight = newExt[2*i+1];
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

  // Add margin for found extreme coordinates
  for(int i=0; i<3; ++i) {
    double diff = newExt[2*i+1] - newExt[2*i];
    if(diff == 0.0) {
      diff = 1.0;
    }
    newExt[2*i] -= diff * _extremeCoordinatesMargin;
    newExt[2*i+1] += diff * _extremeCoordinatesMargin;
  }

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

void Camera::setProjectionParameters() {
  if(_projection == Projection::Orto) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(_extremeCoordinates[0], _extremeCoordinates[1],
            _extremeCoordinates[2], _extremeCoordinates[3],
            -_extremeCoordinates[5], -_extremeCoordinates[4]);
  }
  else {
    // TODO: frustum
  }
}

std::pair<double, double> Camera::getProjectionPlaneSize() const {
  if(_projection == Projection::Orto) {
    double x = _extremeCoordinates[1] - _extremeCoordinates[0];
    double y = _extremeCoordinates[3] - _extremeCoordinates[2];
    return {x, y};
  }

  return {0, 0};
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
