#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>
#include "polyroots.hpp"

using namespace std;

Intersection NonhierSphere::intersect(const Point3D & origin, const Vector3D & ray) {
  // a point is on a sphere if [(p - o) dot (p - o)] = r^2
  // a point is on the ray if p = o + tr
  // substitute for p and solve the qudratic for t

  Vector3D oc = origin - m_pos;

  double a = ray.dot(ray);
  double b = 2*oc.dot(ray);
  double c = oc.dot(oc) - m_radius*m_radius;

  double roots[2];
  size_t num_roots = quadraticRoots(a, b, c, roots);

  double min_t;
  if (num_roots == 1 && roots[0] > EPSILON) {
    min_t = roots[0];
  } else if (num_roots == 2 && (roots[0] > EPSILON || roots[1] > EPSILON)) {
    if      (roots[0] <= EPSILON) min_t = roots[1];
    else if (roots[1] <= EPSILON) min_t = roots[0];
    else                          min_t = min(roots[0], roots[1]);
  } else {
    return Intersection();
  }

  Intersection ret;
  ret.distance = min_t;

  Point3D poi = origin + min_t * ray;
  ret.normal = (poi - m_pos).unit();
  if (ray.dot(ret.normal) > 0) return Intersection(); // you are in the sphere

  return ret;
}

