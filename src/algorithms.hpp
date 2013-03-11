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

#endif // ALGORITHMS_H

