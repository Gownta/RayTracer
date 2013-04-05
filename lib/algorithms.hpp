#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "algebra.hpp"
#include <vector>

using namespace std;

// compute bounding sphere for a mesh
struct BoundingSphere {
  double radius;
  Point3D origin;
};

BoundingSphere find_bounding_sphere(const vector<Point3D> & points);

// return transformation matrices for various simple transformations
Matrix4x4 rotation(double angle, char axis);
Matrix4x4 translation(const Vector3D& displacement);
Matrix4x4 scaling(const Vector3D& scale);

// compute barycentric co-ordinates for p inside a,b,c
Vector3D barycentric(Point3D a, Point3D b, Point3D c, Point3D p);

#endif // ALGORITHMS_H

