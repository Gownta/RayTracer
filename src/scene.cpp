#include "scene.hpp"
#include <iostream>
#include "algorithms.hpp"
#include "primitive.hpp"
#include "program_options.hpp"

///////////////////////////////////////////////////////////////////////////////
// construction

SceneNode::SceneNode(const string & name)
  : m_name(name)

  , m_trans()
  , m_inverse()
  , m_normtrans()

  , m_children()

  , m_bounds()
{}

SceneNode::~SceneNode() {}

///////////////////////////////////////////////////////////////////////////////
// transformations

void SceneNode::rotate(char axis, double angle) {
  update_trans(rotation(angle * 2*M_PI / 360, axis));
}

void SceneNode::rotate(const Matrix4x4 & rot) {
  update_trans(rot);
}

void SceneNode::scale(const Vector3D& amount) {
  update_trans(scaling(amount));
}

void SceneNode::translate(const Vector3D& amount) {
  update_trans(translation(amount));
}

void SceneNode::update_trans(const Matrix4x4 & t) {
  if (cmd_options().count("reverse")) m_trans = m_trans * t;
  else                                m_trans = t * m_trans;
  m_inverse = m_trans.invert();
  m_normtrans = m_inverse.transpose();
}

///////////////////////////////////////////////////////////////////////////////
// intersection

int SceneNode::intersect(const Point3D & _origin, const Vector3D & _ray,
                         IntersectionMode mode, Intersection where[]) const {
  // compute the transformed origin and ray
  Vector3D ray   = get_inverse() * _ray;
  Point3D origin = get_inverse() * _origin;

  // move the origin closer to the object
  // adjust the EXISTENCE bound, if appropriate
  double inct = ((origin - m_bounds.origin).length() - m_bounds.radius) / ray.length() - 1.0;
  if (inct < 2) inct = 0;
  origin = origin + inct * ray;
  if (mode == EXISTENCE) where[0].distance -= inct;

  // try to intersect with the bounding sphere first
  Vector3D uray = ray.unit();
  double closest_s = ((origin - m_bounds.origin) - ((origin - m_bounds.origin).dot(uray) * uray)).length2();
  if (closest_s > m_bounds.radius * m_bounds.radius) return 0;

  // now perform regular intersection
  int k = intersections(origin, ray, mode, where);

  // adjust for inct and reverse transform the normal before returning
  for (int i = 0; i < k; ++i) {
    where[i].distance += inct;
    where[i].normal = (get_normtrans() * where[i].normal).unit();
    assert(where[i].normal.dot(_ray) <= 0);
  }

  // return
  return k;
}

int SceneNode::intersections(const Point3D & origin, const Vector3D & ray,
                             IntersectionMode mode, Intersection where[]) const {
  int ret = 0;
  double closest = INFINITY;
  int closest_index;

  for (auto & child : m_children) {
    int k = child->intersect(origin, ray, mode, where + ret);

    if (k > 0) {
      if (mode == EXISTENCE) return 1;

      ret += k;

      if (mode == CLOSEST) {
        assert(k == 1);
        if (where[ret-1].distance < closest) {
          closest = where[ret-1].distance;
          closest_index = ret-1;
        }
      }
    }
  }

  if (ret <= 1) return ret;
  if (mode == ALL) return ret;

  // mode == CLOSEST
  if (closest_index != 0) where[0] = where[closest_index];
  return 1;
}

int GeometryNode::intersections(const Point3D & origin, const Vector3D & ray,
                                IntersectionMode mode, Intersection where[]) const {
  int ret = m_primitive->intersections(origin, ray, mode, where);
  for (int i = 0; i < ret; ++i) {
    where[i].material = m_material;
  }
  return ret;
}

int CSGNode::intersections(const Point3D & origin, const Vector3D & ray,
                           IntersectionMode mode, Intersection where[]) const {
  // extract the mode == EXISTENCE limit, since we will trample it shortly
  double existence_bound = where[0].distance;

  // compute all intersection points of the children
  // if the operator is + or -, we just need the closest point from m_p1
  // if either is zero, we can optimize
  int k1 = m_p1.intersect(origin, ray, ALL, where);
  int k2 = m_p2.intersect(origin, ray, ALL, where + k1);

  // sort the intersection points
  // if a child is itself a CSGNode, then it will already be sorted
  sort(where, where + k1);
  sort(where + k1, where + k1 + k2);

  // walk through the intersection points
  bool in1 = (k1 % 2);
  bool in2 = (k2 % 2);
  bool ini = in(in1, in2);
  int idx1 = 0;       // where are we in p1's sorted list
  int idx2 = k1;      // where are we in p2's sorted list
  int idx3 = k1 + k2; // where we are storing intersections to return
  for (;;) {
    if (idx1 == k1 && idx2 == k1 + k2) break;

    int next; // the index of the next intersection point we are examining
    if (idx2 == k1 + k2 || (idx1 < k1 && where[idx1].distance < where[idx2].distance)) {
      in1 = !in1;
      next = idx1++;
    } else {
      in2 = !in2;
      next = idx2++;
    }

    bool now_ini = in(in1, in2);
    if (now_ini != ini) { // where[next] is a surface of this CSG
      if (mode == EXISTENCE) {
        // since we are traversing the intersections in order, the first intersection is the closest
        if (where[next].distance < existence_bound) return 1;
        return 0;
      }

      if (mode == CLOSEST) {
        // since we are traversing the intersections in order, the first intersection is the closest
        if (next != 0) where[0] = where[next];
        return 1;
      }

      where[idx3++] = where[next];
      ini = now_ini;
    }
  }

  int ret = idx3 - (k1 + k2);
  
  if (mode == EXISTENCE || mode == CLOSEST) {
    // we should have short-circuited earlier, unless ret is 0
    assert(ret == 0);
    return 0;
  }

  // mode == ALL
  // move the points from [k1+k2, idx3) to [0,??)
  copy(where + k1 + k2, where + idx3, where);
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
// bounds
      
void CSGNode::determine_bounds() {
  m_p1.determine_bounds();
  m_p2.determine_bounds();
}

void SceneNode::determine_bounds() {
  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    node.determine_bounds();
  }
  m_bounds.origin = Point3D(0,0,0);
  m_bounds.radius = INFINITY;
}

void GeometryNode::determine_bounds() {
  set_bounds(m_primitive->get_bounds());
}

