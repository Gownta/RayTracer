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
/*


  // a = ray dot ray, b = 2[(origin - center) dot ray], c = (origin - center) dot (origin - center) - radius^2




  // compute the closest point of the ray to the sphere's origin

  // general formula for closest point on line a + tn to point p is
  //  [n scaled by [(p - a) dot n]] + a
  //  (this assumes that n is a unit vector - scale by |n|^2 if not)


  Point3D closest = origin + (m_pos - origin).dot(ray) / ray.length2() * ray;
  double distance2 = (closest - m_pos).length2();

  double r2 = m_radius * m_radius;


  if (r2 - distance2 >= 0) {
    // the actual point of intersection is not the closest point
    // compute the nearest point on the sphere to the origin, along the ray

    double offset_scale = sqrt(r2 - distance2) / ray.length();
    Vector3D offset = offset_scale * ray;

    Point3D near = closest - offset;

    // the near point is on origin + t*ray
    // t = || (near - origin) || / || ray ||
    // which can be simplified to:
    double t = sqrt((near - origin).length2() / ray.length2());

    if (t > EPSILON) {
      Intersection ret;
      ret.distance = t;
      ret.normal = (near - m_pos);
      ret.normal.normalize();
      // TODO: invert the normal, if necessary
      return ret;
    }

    // we might be inside the circle, and so the intersection is actually the far intersection
    if (t + 2*offset_scale > EPSILON) {
      Intersection ret;
      ret.distance = t + 2*offset_scale;

      Point3D far = closest + offset_scale * ray;
      ret.normal = (far - m_pos);
      ret.normal.normalize();

      cout << "bwuh\n";

      return ret;
    }
  }

  return Intersection();*/
}


