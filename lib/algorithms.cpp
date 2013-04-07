#include "algorithms.hpp"
#include "program_options.hpp"
#include <cassert>

///////////////////////////////////////////////////////////////////////////////
// Bounding Surfaces
///////////////////////////////////////////////////////////////////////////////

static double EXTRA = 1e-5;

// find the minimum bounding sphere (fmbs) of the first k points, using the given boundary points
static BoundingSphere fmbsq(size_t k, const vector<Point3D> & points, const vector<Point3D> & in_boundary);

// find the minimum bounding sphere of the given boundary points
static BoundingSphere fmbsb(const vector<Point3D> & boundary);

// find a point on two planes
static Point3D on_both_planes(const Point3D & p1, const Vector3D & n1, const Point3D & p2, const Vector3D & n2);

// find the weighted bounding sphere
static BoundingSphere fwcs(const vector<Point3D> & points);

///////////////////////////////////////

BoundingSphere find_bounding_sphere(const vector<Point3D> & points) {
  BoundingSphere ret;

  if (cmd_options().count("average-origin-spheres")) {
    ret = fwcs(points);
  } else { // normal case
    ret = fmbsq(points.size(), points, vector<Point3D>());
  }

  // sanity check
  for (vector<Point3D>::const_iterator it = points.begin(); it != points.end(); ++it) {
    assert((*it - ret.origin).length2() <= ret.radius*ret.radius);
  }

  return ret;
}

///////////////////////////////////////

BoundingSphere fmbsq(size_t k, const vector<Point3D> & points, const vector<Point3D> & in_boundary) {
  // base case
  if (k == 0) return fmbsb(in_boundary);

  // mbs of all-but-the-last-point
  BoundingSphere candidate = fmbsq(k-1, points, in_boundary);

  // if the last point is in, we're good
  if ((points[k-1] - candidate.origin).length2() <= candidate.radius*candidate.radius) return candidate;

  // otherwise, the last point is in the boundary
  vector<Point3D> new_boundary = in_boundary;
  new_boundary.push_back(points[k-1]);
  return fmbsq(k-1, points, new_boundary);
}

BoundingSphere fmbsb(const vector<Point3D> & boundary) {
  assert(boundary.size() <= 4);
  BoundingSphere ret;

  switch (boundary.size()) {
    case 4: {
      Vector3D half1 = 0.5 * (boundary[1] - boundary[0]);
      Point3D mid01 = boundary[0] + half1;
      Vector3D mid01norm = half1.unit();
      
      Vector3D half2 = 0.5 * (boundary[2] - boundary[0]);
      Point3D mid02 = boundary[0] + half2;
      Vector3D mid02norm = half2.unit();

      Point3D on_both = on_both_planes(mid01, mid01norm, mid02, mid02norm);
      Vector3D dir = mid01norm.cross(mid02norm).unit();

      Vector3D half3 = 0.5 * (boundary[3] - boundary[0]);
      Point3D mid03 = boundary[0] + half3;
      Vector3D mid03norm = half3.unit();
      
      // where does on_both+t*dir intersect plane (mid03, mid03norm)?
      // http://en.wikipedia.org/wiki/Line-plane_intersection
      double t = (1.0 / mid03norm.dot(dir)) * ((mid03 - on_both).dot(mid03norm));

      ret.origin = on_both + t * dir;
      ret.radius = (ret.origin - boundary[0]).length() + EXTRA;
    } break;
    case 3: {
      Vector3D half1 = 0.5 * (boundary[1] - boundary[0]);
      Point3D mid01 = boundary[0] + half1;
      Vector3D mid01norm = half1.unit();
      
      Vector3D half2 = 0.5 * (boundary[2] - boundary[0]);
      Point3D mid02 = boundary[0] + half2;
      Vector3D mid02norm = half2.unit();

      Point3D on_both = on_both_planes(mid01, mid01norm, mid02, mid02norm);
      Vector3D dir = mid01norm.cross(mid02norm).unit();

      ret.origin = on_both + dir.dot(boundary[0] - on_both) * dir;
      ret.radius = (ret.origin - boundary[0]).length() + EXTRA;
    } break;
    case 2: {
      Vector3D half = 0.5 * (boundary[1] - boundary[0]);
      ret.radius = half.length() + EXTRA;
      ret.origin = boundary[0] + half;
    } break;
    case 1: {
      ret.radius = 0 + EXTRA;
      ret.origin = boundary.front();
    } break;
    case 0: default: {
      ret.radius = 0;
      ret.origin = Point3D(0,0,0);
    } break;
  }

  // sanity check
  for (vector<Point3D>::const_iterator it = boundary.begin(); it != boundary.end(); ++it) {
    assert((*it - ret.origin).length2() <= ret.radius*ret.radius);
  }

  return ret;
}

///////////////////////////////////////

Point3D on_both_planes(const Point3D & p1, const Vector3D & n1, const Point3D & p2, const Vector3D & n2) {
  // intersection's direction
  Vector3D dir = n1.cross(n2).unit();

  // direction from p1 to plane2
  Vector3D to = n1.cross(dir).unit();

  // where does line p1+t*to meet plane2?
  // http://en.wikipedia.org/wiki/Line-plane_intersection
  double t = (1.0 / n2.dot(to)) * ((p2 - p1).dot(n2));

  Point3D ret = p1 + t*to;

  assert(abs((ret - p1).dot(n1)) < 1e-5);
  assert(abs((ret - p2).dot(n2)) < 1e-5);

  return ret;
}

///////////////////////////////////////

BoundingSphere fwcs(const vector<Point3D> & points) {
  Vector3D acc(0,0,0);
  for (vector<Point3D>::const_iterator it = points.begin(); it != points.end(); ++it) {
    acc = acc + (*it - Point3D(0,0,0));
  }
  acc = (1.0 / points.size()) * acc;
  
  Point3D origin(acc[0], acc[1], acc[2]);

  double maxR = 0;
  for (vector<Point3D>::const_iterator it = points.begin(); it != points.end(); ++it) {
    double nR = (*it - origin).length();
    maxR = max(maxR, nR);
  }

  BoundingSphere ret;
  ret.origin = origin;
  ret.radius = maxR + EXTRA;
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
// Matrix Helpers
///////////////////////////////////////////////////////////////////////////////

Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 r;
  
  double c = cos(angle);
  double s = sin(angle);

  if (axis == 'z' || axis == 'Z') {
    r(0,0) = c;
    r(0,1) = -s;
    r(1,0) = s;
    r(1,1) = c;
  } else if (axis == 'y' || axis == 'Y') {
    r(0,0) = c;
    r(0,2) = s;
    r(2,0) = -s;
    r(2,2) = c;
  } else if (axis == 'x' || axis == 'X') {
    r(1,1) = c;
    r(1,2) = -s;
    r(2,1) = s;
    r(2,2) = c;
  }

  return r;
}

Matrix4x4 translation(const Vector3D & d) {
  Matrix4x4 t;
  t(0, 3) = d[0];
  t(1, 3) = d[1];
  t(2, 3) = d[2];
  return t;
}

Matrix4x4 scaling(const Vector3D & v) {
  Matrix4x4 s;
  s(0, 0) = v[0];
  s(1, 1) = v[1];
  s(2, 2) = v[2];
  return s;
}

///////////////////////////////////////////////////////////////////////////////
// Barycentric Calculations
///////////////////////////////////////////////////////////////////////////////

Vector3D barycentric(Point3D a, Point3D b, Point3D c, Point3D p) {
  Vector3D ab = b - a;
  Vector3D ac = c - a;
  Vector3D ap = p - a;

  double iA = 1.0 / (0.5 * ab.cross(ac).length());
  double w = 0.5 * ab.cross(ap).length() * iA;
  double v = 0.5 * ac.cross(ap).length() * iA;
  double u = 1.0 - v - w;

  return Vector3D(u, v, w);
}

///////////////////////////////////////////////////////////////////////////////
// Random Normals
///////////////////////////////////////////////////////////////////////////////

Vector3D random_normal() {
  // The area of a polar cap of a sphere is 2*pi*r*h, where h is the height
  // of the cap. This implies that a random point on the sphere has a height
  // which is uniformly distributed across [-1,1].
  // A random longitude has angle randomly in [0,360)
  double theta = (rand() % 3600) / 3600.0 * 2*M_PI;
  double x = cos(theta);
  double z = sin(theta);
  double y = ((rand() % 1048576) / 1048576.0 - 0.5) * 2;
  double scale = sqrt(1 - y*y);
  return Vector3D(x*scale, y, z*scale);
}

