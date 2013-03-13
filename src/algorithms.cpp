#include "algorithms.hpp"
#include <cassert>

static double EXTRA = 1e-5;

// find the minimum bounding sphere (fmbs) of the first k points, using the given boundary points
static BoundingSphere fmbsq(size_t k, const vector<Point3D> & points, const vector<Point3D> & in_boundary);
static BoundingSphere fmbsb(const vector<Point3D> & boundary);
static Point3D on_both_planes(const Point3D & p1, const Vector3D & n1, const Point3D & p2, const Vector3D & n2);

static BoundingSphere fwcs(const vector<Point3D> & points);

BoundingSphere find_minimal_bounding_sphere(const vector<Point3D> & points) {
  BoundingSphere ret = fmbsq(points.size(), points, vector<Point3D>());

  //BoundingSphere ret = fwcs(points);
  //cout << ret.radius << endl;

  // sanity check
  for (vector<Point3D>::const_iterator it = points.begin(); it != points.end(); ++it) {
    assert((*it - ret.origin).length2() <= ret.radius*ret.radius);
  }

  return ret;
}

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

  for (vector<Point3D>::const_iterator it = boundary.begin(); it != boundary.end(); ++it) {
    /*Vector3D delta = *it - ret.origin;
    if (delta.length() > ret.radius) {
      cout << "boundary pt = " << *it << "\n"
           << "origin      = " << ret.origin << "\n"
           << "radius      = " << ret.radius << "\n"
           << "delta       = " << delta.length() << "\n"
           << "difference  = " << (delta.length() - ret.radius) << "\n"
           << "elems       = " << boundary.size() << "\n"
           << endl;
    }*/
    assert((*it - ret.origin).length2() <= ret.radius*ret.radius);
  }

  return ret;
}

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

// weighted center
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

