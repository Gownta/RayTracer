#include "scene.hpp"
#include <iostream>
#include "algorithms.hpp"
#include "primitive.hpp"

///////////////////////////////////////////////////////////////////////////////
// construction

SceneNode::SceneNode(const string & name)
  : m_name(name)

  , m_trans()
  , m_inverse()
  , m_normtrans()

  , m_parent(0)
  , m_children()

  //, m_bounding_radius(INFINITY)
{}

SceneNode::~SceneNode() {}

///////////////////////////////////////////////////////////////////////////////
// transformations

void SceneNode::rotate(char axis, double angle) {
  //m_trans = m_trans * rotation(angle * 2*M_PI / 360, axis);
  m_trans = rotation(angle * 2*M_PI / 360, axis) * m_trans;
  update_trans();
}

void SceneNode::rotate(const Matrix4x4 & rot) {
  //m_trans = m_trans * rot;
  m_trans = rot * m_trans;
  update_trans();
}

void SceneNode::scale(const Vector3D& amount) {
  //m_trans = m_trans * scaling(amount);
  m_trans = scaling(amount) * m_trans;
  update_trans();
}

void SceneNode::translate(const Vector3D& amount) {
  //m_trans = m_trans * translation(amount);
  m_trans = translation(amount) * m_trans;
  update_trans();
}

void SceneNode::update_trans() {
  m_inverse = m_trans.invert();
  m_normtrans = m_inverse.transpose();
}

///////////////////////////////////////////////////////////////////////////////
// intersection

Intersection SceneNode::intersect(const Point3D & _origin, const Vector3D & _ray) {
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  Intersection closest;

  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    Intersection candidate = node.intersect(origin, ray);
    if (candidate < closest) closest = candidate;
  }

  if (closest) {
    closest.normal = (get_normtrans() * closest.normal).unit();
  }

  return closest;
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

Intersection GeometryNode::intersect(const Point3D & _origin, const Vector3D & _ray) {
  // perform the inverse transformation to get the ray from the primitives' perspective
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  /*
  cout << "_origin = " << _origin << "\n";
  cout << " origin = " <<  origin << "\n";
  cout << "   _ray = " << _ray << "\n";
  cout << "    ray = " <<  ray << "\n";
  cout << get_trans() << "\n";
  cout << get_inverse() << "\n\n";
  */

  // try to intersect with the bounding sphere first
  Vector3D uray = ray.unit();
  double closest_s = ((origin - m_bounds.origin) - ((origin - m_bounds.origin).dot(uray) * uray)).length2();
  if (closest_s > m_bounds.radius * m_bounds.radius) return Intersection();

  // intersect with the primitive
  Intersection result;
  if (1) {
    result = m_primitive->intersect(origin, ray);
  } else {
    // draw the bounding region
    //NonhierSphere nhs(m_bounds.origin, m_bounds.radius);
    //result = nhs.intersect(origin, ray);
  }

  // need to set the material and adjust the normal
  if (result) {
    result.material = m_material;
    result.normal = (get_normtrans() * result.normal).unit();
    assert(result.normal.dot(_ray) < 0);
    return result;
  }

  return Intersection();
}

void GeometryNode::determine_bounds() {
  //m_bounding_radius = m_primitive->get_bounding_radius();
  m_bounds = m_primitive->get_bounds();
}

