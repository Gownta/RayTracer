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
{}

SceneNode::~SceneNode() {}

///////////////////////////////////////////////////////////////////////////////
// transformations

void SceneNode::rotate(char axis, double angle) {
  if (cmd_options().count("reverse")) m_trans = m_trans * rotation(angle * 2*M_PI / 360, axis);
  else                                m_trans = rotation(angle * 2*M_PI / 360, axis) * m_trans;
  update_trans();
}

void SceneNode::rotate(const Matrix4x4 & rot) {
  if (cmd_options().count("reverse")) m_trans = m_trans * rot;
  else                                m_trans = rot * m_trans;
  update_trans();
}

void SceneNode::scale(const Vector3D& amount) {
  if (cmd_options().count("reverse")) m_trans = m_trans * scaling(amount);
  else                                m_trans = scaling(amount) * m_trans;
  update_trans();
}

void SceneNode::translate(const Vector3D& amount) {
  if (cmd_options().count("reverse")) m_trans = m_trans * translation(amount);
  else                                m_trans = translation(amount) * m_trans;
  update_trans();
}

void SceneNode::update_trans() {
  m_inverse = m_trans.invert();
  m_normtrans = m_inverse.transpose();
}

///////////////////////////////////////////////////////////////////////////////
// intersection

int SceneNode::intersections(const Point3D & _origin, const Vector3D & _ray,
                             IntersectionMode mode, Intersection where[]) const {
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  int ret = 0;
  double closest = INFINITY;
  int closest_index;

  for (auto & child : m_children) {
    int k = child->intersections(origin, ray, mode, where + ret);
    if (k > 0) {
      if (mode == EXISTENCE) return 1;

      for (int i = ret; i < ret + k; ++i) {
        where[i].normal = (get_normtrans() * where[i].normal).unit();
        assert(where[i].normal.dot(_ray) <= 0);
      }
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

void SceneNode::determine_bounds() {
  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    node.determine_bounds();
  }
  //m_bounding_radius = 0;
  /*
  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    node.determine_bounds();
    double max_scale = max(node.m_scale[0], max(node.m_scale[1], node.m_scale[2]));
    double trans_dist = m_translate.length();
    double max_bound = max_scale * node.m_bounding_radius + trans_dist;
    m_bounding_radius = max(m_bounding_radius, max_bound);
  }*/
}

int GeometryNode::intersections(const Point3D & _origin, const Vector3D & _ray,
                                IntersectionMode mode, Intersection where[]) const {
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  // try to intersect with the bounding sphere first
  Vector3D uray = ray.unit();
  double closest_s = ((origin - m_bounds.origin) - ((origin - m_bounds.origin).dot(uray) * uray)).length2();
  if (closest_s > m_bounds.radius * m_bounds.radius) return 0;

  int ret = m_primitive->intersections(origin, ray, mode, where);
  for (int i = 0; i < ret; ++i) {
    where[i].material = m_material;
    where[i].normal = (get_normtrans() * where[i].normal).unit();
    assert(where[i].normal.dot(_ray) < 0);
  }

  return ret;
}

void GeometryNode::determine_bounds() {
  m_bounds = m_primitive->get_bounds();
}

///////////////////////////////////////////////////////////////////////////////

int CSGNode::intersections(const Point3D & _origin, const Vector3D & _ray,
                           IntersectionMode mode, Intersection where[]) const {
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  // try to intersect with the bounding sphere first

  // there exists an optimization for (mode == EXISTENCE && m_op == UNION)

  int k1 = m_p1.intersections(origin, ray, ALL, where);
  int k2 = m_p2.intersections(origin, ray, ALL, where + k1);

  // if a child is itself a CSGNode, then it will already be sorted

  sort(where, where + k1);
  sort(where + k1, where + k1 + k2);

  // walk through the intersection points
  bool in1 = (k1 % 2);
  bool in2 = (k2 % 2);
  bool old_inside;
    switch (m_op) {
      case UNION:        old_inside = in1 ||  in2; break;
      case INTERSECTION: old_inside = in1 &&  in2; break;
      case DIFFERENCE:   old_inside = in1 && !in2; break;
    }
  int idx1 = 0;       // where are we in g1's sorted list
  int idx2 = k1;      // where are we in g2's sorted list
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

    where[next].normal = (get_normtrans() * where[next].normal).unit();
    assert(where[next].normal.dot(_ray) < 0);

    bool now_inside;
    switch (m_op) {
      case UNION:        now_inside = in1 ||  in2; break;
      case INTERSECTION: now_inside = in1 &&  in2; break;
      case DIFFERENCE:   now_inside = in1 && !in2; break;
    }

    if (old_inside != now_inside) {
      if (mode == EXISTENCE) return 1;
      if (mode == CLOSEST) { // since we are processing the points in sorted order, the first time we are inside is the closest
        if (next != 0) where[0] = where[next];
        return 1;
      }

      where[idx3++] = where[next];
      old_inside = now_inside;
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

void CSGNode::determine_bounds() {
  m_p1.determine_bounds();
  m_p2.determine_bounds();
}

