#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>
#include "polyroots.hpp"

using namespace std;

Algebraic::Algebraic(const string & eqn, double radius)
    : m_eqn(parse_equation(eqn, true))
    , m_deg(0)
    , m_br(radius)
{
  for (const auto & f : m_eqn) {
    m_deg = max(m_deg, f.pow_t);
  }

  auto unex = parse_equation(eqn, false);

  for (const auto & f : unex) {
    if (f.pow_x > 0) {
      m_dx.push_back(f);
      m_dx.back().k *= f.pow_x;
      m_dx.back().pow_x--;
    }
    if (f.pow_y > 0) {
      m_dy.push_back(f);
      m_dy.back().k *= f.pow_y;
      m_dy.back().pow_y--;
    }
    if (f.pow_z > 0) {
      m_dz.push_back(f);
      m_dz.back().k *= f.pow_z;
      m_dz.back().pow_z--;
    }
  }
}

Intersection Algebraic::intersect(const Point3D & _origin, const Vector3D & ray) const {
  // for numerical stability, bring the origin closer to the bounding volume
  // add inct to distance before returning the point of intersection
  double inct = (Vector3D(_origin).length() - m_br) / ray.length();
  if (inct < 0) inct = 0;
  Point3D origin = _origin + inct*ray;

  double c[5] = { 0 };
  for (const auto & f : m_eqn) {
    c[f.pow_t] += f.k
                  * pow(origin[0], f.pow_ox) 
                  * pow(origin[1], f.pow_oy) 
                  * pow(origin[2], f.pow_oz) 
                  * pow(ray[0], f.pow_rx)
                  * pow(ray[1], f.pow_ry)
                  * pow(ray[2], f.pow_rz);
                
  }

  double roots[4];
  size_t num_roots;

  switch (m_deg) {
    case 4: num_roots = quarticRoots  (c[3] / c[4], c[2]/c[4], c[1]/c[4], c[0]/c[4], roots); break;
    case 3: num_roots = cubicRoots    (c[2] / c[3], c[1]/c[3], c[0]/c[3], roots); break;
    case 2: num_roots = quadraticRoots(c[2], c[1], c[0], roots); break;
    default: 
      cout << m_deg << endl;
      assert(false);
  }

  Intersection closest;

  for (size_t i = 0; i < num_roots; ++i) {
    double root = roots[i];
    if (root > 1e-5 && root < closest.distance) {
      closest.distance = root;

      Point3D at  = origin + root * ray;
      double dx = 0, dy = 0, dz = 0;
      for (const auto & f : m_dx) {
        dx += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      for (const auto & f : m_dy) {
        dy += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      for (const auto & f : m_dz) {
        dz += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      closest.normal = Vector3D(dx, dy, dz).unit();

      double dot = closest.normal.dot(ray);
      if (dot > 0 && dot < 1e-5) closest.normal = -closest.normal;

      /*cout << "line " << origin << " + t*" << ray << "\n"
           << "intersected the torus at t = " << closest.distance << ", i.e. point " << at << "\n"
           << "the normal was " << closest.normal << "\n\n";*/
    }
  }

  closest.distance += inct;
  return closest;
}

BoundingSphere Algebraic::get_bounds() const {
  BoundingSphere ret;
  ret.origin = Point3D(0,0,0);
  ret.radius = m_br + 1e-5;
  return ret;
}







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
  if (abs(dot) >= EPSILON) {
    // where does origin+t*uray intersect the top and bottom circle
    double t1 = (origin - Point3D(0, 1,0)).dot(Vector3D(0,1,0)) / -dot;
    double t2 = (origin - Point3D(0,-1,0)).dot(Vector3D(0,1,0)) / -dot;

    if (t1 > EPSILON && t1 < closest.distance) {
      Point3D pos = origin + t1*uray;
      if ((pos - Point3D(0,1,0)).length2() <= 1.0) {
        closest.distance = t1;
        closest.normal = Vector3D(0,1,0);
      }
    }

    if (t2 > EPSILON && t2 < closest.distance) {
      Point3D pos = origin + t2*uray;
      if ((pos - Point3D(0,-1,0)).length2() <= 1.0) {
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

  for (size_t i = 0; i < num_roots; ++i) {
    double root = roots[i];
    Point3D at  = origin + root * uray;
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
  ret.radius = sqrt(2) + 1e-5;
  return ret;
}

