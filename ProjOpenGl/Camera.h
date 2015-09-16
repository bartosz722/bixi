#ifndef PROJOPENGL_CAMERA_H_
#define PROJOPENGL_CAMERA_H_

#include <array>
#include <utility>
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

  void addToFrustumNear(double value);
  void updateView(const Universe::Snapshot & s);

  std::pair<double, double> getProjectionPlaneSize() const;
  double getGreatestCoordinateDifference() const;

private:
  typedef std::array<double, 6> ExtremeCoordinates; // xl, xr, yb, yt, zf, zn

  // returns: "visible" object found
  bool getExtremeCoordinates(ExtremeCoordinates & result, bool useFrustumNearMin);
  void updateExtremeCoordinates(bool useFrustumNearMin);
  void updateFrustumParameters();
  void setProjectionParameters();
  void storeModelViewMatrixAndLoad1();
  void multiplyByStoredModelViewMatrix();

  static constexpr double _extremeCoordinatesMargin = 0.05; // fraction of extreme coordinate
  static constexpr double _frustumNearMin = 0.001;

  bool _followAllObjects;
  Projection _projection;
  bool _updateProjection;
  ExtremeCoordinates _extremeCoordinates;
  bool _extremeCoordinatesInitialized;
  double _frustumNear;
  double _frustumLeft;
  double _frustumDown;
  bool _frustumParemetersInitialized;
  const PhysicalObjectsContainer * _currObjects;
  std::pair<double, double> _projectionPlaneSize;
  double storedModelViewMatrix[16];
};

#endif /* PROJOPENGL_CAMERA_H_ */
