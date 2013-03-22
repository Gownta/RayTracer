#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "algebra.hpp"
#include <vector>

using namespace std;

struct BoundingSphere {
  double radius;
  Point3D origin;
};

BoundingSphere find_minimal_bounding_sphere(const vector<Point3D> & points);

// return transformation matrices for various simple transformations
Matrix4x4 rotation(double angle, char axis);
Matrix4x4 translation(const Vector3D& displacement);
Matrix4x4 scaling(const Vector3D& scale);

#endif // ALGORITHMS_H

