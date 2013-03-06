#include "a2.hpp"
#include <cmath>

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 r;
  
  double c = cos(angle);
  double s = sin(angle);

  if (axis == 'z') {
    r(0,0) = c;
    r(0,1) = -s;
    r(1,0) = s;
    r(1,1) = c;
  } else if (axis == 'y') {
    r(0,0) = c;
    r(0,2) = s;
    r(2,0) = -s;
    r(2,2) = c;
  } else if (axis == 'x') {
    r(1,1) = c;
    r(1,2) = -s;
    r(2,1) = s;
    r(2,2) = c;
  }

  // Fill me in!
  return r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& d)
{
  Matrix4x4 t;
  t(0, 3) = d[0];
  t(1, 3) = d[1];
  t(2, 3) = d[2];
  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& v)
{
  Matrix4x4 s;
  s(0, 0) = v[0];
  s(1, 1) = v[1];
  s(2, 2) = v[2];
  return s;
}
