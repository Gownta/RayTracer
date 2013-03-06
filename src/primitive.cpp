#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>

using namespace std;

Intersection NonhierSphere::intersect(const Point3D & origin, const Vector3D & ray) {
  // compute the closest point of the ray to the sphere's origin

  // general formula for closest point on line a + tn to point p is
  //  [n scaled by [(p - a) dot n]] + a
  //  (this assumes that n is a unit vector - scale by |n|^2 if not)

  /*cout << "in Intersect: origin    = " << origin << "\n"
       << "              ray       = " << ray << endl;*/

  Point3D closest = origin + (m_pos - origin).dot(ray) / ray.length2() * ray;
  double distance2 = (closest - m_pos).length2();

  double r2 = m_radius * m_radius;

  /*cout << "              closest   = " << closest << "\n"
       << "              distance2 = " << distance2 << "\n"
       << "              radius2   = " << r2 << "\n";*/

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

      return ret;
    }
  }

  return Intersection();
}


