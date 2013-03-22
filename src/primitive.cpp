#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>
#include "polyroots.hpp"

using namespace std;

Intersection NonhierSphere::intersect(const Point3D & origin, const Vector3D & ray) const {
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

BoundingSphere NonhierSphere::get_bounds() const {
  BoundingSphere ret;
  ret.origin = m_pos;
  ret.radius = m_radius + 1e-5;
  return ret;
}

Intersection Cylinder::intersect(const Point3D & origin, const Vector3D & uray) {
  Intersection closest;

  double dot = uray.dot(Vector3D(0,1,0));
  if (abd(dot) >= EPSILON) {
    // where does origin+t*uray intersect the top and bottom circle
    double t1 = (origin - Point3D(0, 1,0)).dot(Vector3D(0,1,0)) / -dot;
    double t2 = (origin - Point3D(0,-1,0)).dot(Vector3D(0,1,0)) / -dot;

    if (t1 > EPSILON && t1 < closest.distance) {
      Point3D pos = origin + t*uray;
      if ((pos - Point(0,1,0)).length2() <= 1.0) {
        closest.t = t1;
        closest.normal = Vector3D(0,1,0);
      }
    }

    if (t2 > EPSILON && t2 < closest.distance) {
      Point3D pos = origin + t*uray;
      if ((pos - Point(0,-1,0)).length2() <= 1.0) {
        closest.distance = t2;
        closest.normal = Vector3D(0,-1,0);
      }
    }
  }

  // where does origin+t*uray intersect the infinite cyliner?
  double a = uray[0]*uray[0] + uray[2]*uray[2];
  double b = 2*(origin[0]*uray[0] + origin[2]*uray[2]);
  double c = origin[0]*origin[0] + origin[2]*origin[2] - 1;

  double roots[2];
  size_t num_roots = quadraticRoots(a, b, c, roots);

  for (int i = 0; i < num_roots; ++i) {
    double root = roots[i];
    Point3D at  = origin + t * uray;
    if (abs(at[1]) <= 1 && root < closest.distance) {
      closest.distance = root;
      closest.normal = Vector3D(at[0], 0, at[2]);
    }
  }

  return closest;
}

BoundingSphere Cylinder::get_bounds() {
  BoundingSphere ret;
  ret.origin = Point3D(0,0,0);
  ret.radius = sqrt(2) + EXTRA;
  return ret;
}

