#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "algebra.hpp"
#include <vector>

using namespace std;

// compute bounding sphere for a mesh
struct BoundingSphere {
  BoundingSphere(double r = INFINITY, const Point3D & o = Point3D(0,0,0)) : radius(r), origin(o) {}
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

// compute a point on the unit sphere
Vector3D random_normal();

#endif // ALGORITHMS_H

