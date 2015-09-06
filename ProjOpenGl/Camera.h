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

  Camera();

  void translate();
  void rotate();

  void updateView(const Universe::Snapshot & s);

  std::pair<double, double> getProjectionPlaneSize() const;
  double getGreatestCoordinateDifference() const;

private:
  typedef std::array<double, 6> ExtremeCoordinates; // xl, xr, yb, yt, zf, zn

  void updateExtremeCoordinates(const Universe::Snapshot & s);
  void setProjectionParameters();

  static constexpr double _extremeCoordinatesMargin = 0.05; // % of extreme coordinate

  bool _followAllObjects;
  Projection _projection;
  bool _updateProjection;
  bool _firstUpdateViewCall;
  ExtremeCoordinates _extremeCoordinates;
  bool _extremeCoordinatesInitialized;
};

#endif /* PROJOPENGL_CAMERA_H_ */
