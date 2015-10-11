#ifndef PROJOPENGL_CAMERA_H_
#define PROJOPENGL_CAMERA_H_

#include <array>
#include <utility>
#include <sstream>
#include <glm/glm.hpp>
#include "Universe.h"

class Camera {
public:
  enum class Projection {
    Orto,
    Frustum,
  };

  enum class Axis { X, Y, Z };

  Camera();

  void translate(Axis a, double factor);
  void rotate(Axis a, double angleDeg);

  void setFollowAllObjects(bool f);
  bool getFollowAllObjects() const { return _followAllObjects; }

  void setProjection(Projection p);
  Projection getProjection() const { return _projection; }

  void changeFrustumNear(double factor);
  void changeFrustumFar(double factor);
  void updateView(const Universe::Snapshot & s);

  std::pair<double, double> getProjectionPlaneSize() const;
  double getGreatestCoordinateDifference() const;

private:
  struct ExtremeCoordinates {
    ExtremeCoordinates() { _valid = false; _coord.fill(0); }
    bool _valid;
    std::array<double, 6> _coord; // { xl, xr, yb, yt, zf, zn }; zn >= zf
  };

  struct ProjParams {
    ProjParams() { _valid = false; _left = _right = _bottom = _top = _near = _far = 0; }
    std::string toString() {
      if(_valid) {
        std::stringstream buf;
        buf << "(l " << _left << ", r " << _right << ", b " << _bottom
            << ", t " << _top << ", n " << _near << ", f " << _far << ")";
        return buf.str();
      }
      return "invalid";
    }
    bool _valid;
    double _left, _right;
    double _bottom, _top;
    double _near, _far;
  };

  void readExtremeCoordinates();
  void setOptimalPositionForCamera();
  void calculateProjectionParameters();
  void useProjectionParameters();
  void storeModelViewMatrixAndLoad1();
  void multiplyByStoredModelViewMatrix();
  bool projectionParametersValid();
  ProjParams getOrtoParamsToSeeAll();
  ProjParams getFrustumParamsToSeeAllInFrontOfCamera();
  void translateWorld(const glm::dvec3 & tv);
  void rotateWorld(double angleDeg, const glm::dvec3 & rv);
  void loadModelViewMatrixToCache();
  glm::dvec4 cachedModelViewMatrix_Mul_Vector(const Vector & v);

  static constexpr double _extremeCoordinatesMargin = 0.05; // fraction of extreme coordinate
  static constexpr double _frustumNearMin = 0.001;
  static constexpr double _optimalCameraZPositionFactor = 1.0;

  bool _followAllObjects;
  Projection _projection;
  bool _updateGlProjection;

  ExtremeCoordinates _extremeCoordinates;
  ProjParams _ortoParams;
  ProjParams _frustumParams;

  const PhysicalObjectsContainer * _currObjects;
  std::pair<double, double> _projectionPlaneSize;
  double _storedModelViewMatrix[16];
  glm::dmat4x4 _cachedModelViewMatrix;
};

#endif /* PROJOPENGL_CAMERA_H_ */
