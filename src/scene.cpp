#include "scene.hpp"
#include <iostream>
#include "a2.hpp"

SceneNode::SceneNode(const string & name)
  : m_name(name)
  , m_scale(1,1,1)
  , m_translate()
  , m_rotate()
  , m_trans()
  , m_invtrans()
  , m_invtranspose()
  , m_parent(0)
  , m_children()
  , m_bounding_radius(INFINITY)
{}

SceneNode::~SceneNode() {}

void SceneNode::rotate(char axis, double angle) {
  m_rotate = rotation(angle * 2*M_PI / 360, axis) * m_rotate;
  update_trans();
}

void SceneNode::rotate(const Matrix4x4 & rot) {
  m_rotate = rot * m_rotate;
  update_trans();
}

void SceneNode::scale(const Vector3D& amount) {
  for (int i = 0; i < 3; ++i) m_scale[i] *= amount[i];
  update_trans();
}

void SceneNode::translate(const Vector3D& amount) {
  m_translate = m_translate + amount;
  update_trans();
}

Matrix4x4 SceneNode::get_local_transform() const {
  return translation(m_translate) * m_rotate * scaling(m_scale);
}

void SceneNode::update_trans() {
  m_trans = get_local_transform(); //translation(m_translate) * m_rotate * scaling(m_scale);
  if (m_parent != NULL) m_trans = m_parent->get_transform() * m_trans;
  m_invtrans = m_trans.invert();
  m_invtranspose = m_invtrans.transpose();
}

void SceneNode::determine_bounds() {
  m_bounding_radius = 0;
  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    node.determine_bounds();
    double max_scale = max(node.m_scale[0], max(node.m_scale[1], node.m_scale[2]));
    double trans_dist = m_translate.length();
    double max_bound = max_scale * node.m_bounding_radius + trans_dist;
    m_bounding_radius = max(m_bounding_radius, max_bound);
  }
}

Intersection GeometryNode::intersect(const Point3D & _origin, const Vector3D & _ray) {
  // perform the inverse transformation to get the ray from the primitives' perspective
  Point3D  origin = get_inverse() * _origin;
  Vector3D ray    = get_inverse() * _ray;

  // try to intersect with the bounding sphere first
  //double closest_s = ((origin - Point3D(0,0,0)) - ((origin - Point3D(0,0,0)).dot(ray) * ray)).length2();
  //if (closest_s > m_bounding_radius * m_bounding_radius) return Intersection();

  // intersect with the primitives
  // for now, just one primitive
  Intersection result = m_primitive->intersect(origin, ray);

  // need to set the material and adjust the normal
  if (result) {
    result.material = m_material;
    result.normal = (get_inverse_transpose() * result.normal).unit();
    assert(result.normal.dot(_ray) < 0);
    return result;
  }

  return Intersection();
}

void GeometryNode::determine_bounds() {
  m_bounding_radius = m_primitive->get_bounding_radius();
}

