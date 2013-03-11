#include "scene.hpp"
#include <iostream>
#include "a2.hpp"

SceneNode::SceneNode(const string & name)
  : m_name(name)

  /*, m_scale(1,1,1)
  , m_translate()
  , m_rotate()
  , m_trans()
  , m_trans2()
  , m_invtrans()
  , m_invtranspose()*/
  , m_local_trans()
  , m_global_trans()
  , m_global_trans_inverse()
  , m_global_trans_inverse_transpose()

  , m_parent(0)
  , m_children()
  , m_bounding_radius(INFINITY)
{}

SceneNode::~SceneNode() {}

void SceneNode::rotate(char axis, double angle) {
  /*// wrong order?
  m_rotate = rotation(angle * 2*M_PI / 360, axis) * m_rotate;
  m_trans2 = m_trans2 * rotation(angle * 2*M_PI / 360, axis);
  update_trans();*/
  m_local_trans = m_local_trans * rotation(angle * 2*M_PI / 360, axis);
}

void SceneNode::rotate(const Matrix4x4 & rot) {
/*
  // wrong?
  m_rotate = rot * m_rotate;
  m_trans2 = m_trans2 * rot;
  update_trans();
*/
  m_local_trans = m_local_trans * rot;
}

void SceneNode::scale(const Vector3D& amount) {
/*
  for (int i = 0; i < 3; ++i) m_scale[i] *= amount[i];
  m_trans2 = m_trans2 * scaling(amount);
  update_trans();
  */
  m_local_trans = m_local_trans * scaling(amount);
}

void SceneNode::translate(const Vector3D& amount) {
/*
  m_translate = m_translate + amount;
  m_trans2 = m_trans2 * translation(amount);
  update_trans();
*/
  m_local_trans = m_local_trans * translation(amount);
}

/*Matrix4x4 SceneNode::get_local_transform() const {
  return m_trans2;
  return translation(m_translate) * m_rotate * scaling(m_scale);
}*/

/*void SceneNode::update_trans() {
  m_trans = get_local_transform(); //translation(m_translate) * m_rotate * scaling(m_scale);
  if (m_parent != NULL) m_trans = m_parent->get_transform() * m_trans;
  m_invtrans = m_trans.invert();
  m_invtranspose = m_invtrans.transpose();
}*/

void SceneNode::setup_global_trans() {
  if (m_parent == NULL) m_global_trans = m_local_trans;
  else                  m_global_trans = m_parent->get_global_trans() * m_local_trans;

  m_global_trans_inverse = m_global_trans.invert();
  m_global_trans_inverse_transpose = m_global_trans_inverse.transpose();

  for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
    SceneNode & node = **it;
    node.setup_global_trans();
  }
}

void SceneNode::determine_bounds() {
  m_bounding_radius = 0;
  /*for (ChildList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
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
  /*Point3D  origin = get_inverse() * _origin;
  Vector3D ray    = get_inverse() * _ray;
  */
  Point3D origin = get_global_inverse() * _origin;
  Vector3D ray   = get_global_inverse() * _ray;

  // try to intersect with the bounding sphere first
  //double closest_s = ((origin - Point3D(0,0,0)) - ((origin - Point3D(0,0,0)).dot(ray) * ray)).length2();
  //if (closest_s > m_bounding_radius * m_bounding_radius) return Intersection();

  // intersect with the primitives
  // for now, just one primitive
  Intersection result = m_primitive->intersect(origin, ray);

  // need to set the material and adjust the normal
  if (result) {
    result.material = m_material;
    //result.normal = (get_inverse_transpose() * result.normal).unit();
    result.normal = (get_global_normtrans() * result.normal).unit();
    assert(result.normal.dot(_ray) < 0);
    return result;
  }

  return Intersection();
}

void GeometryNode::determine_bounds() {
  m_bounding_radius = m_primitive->get_bounding_radius();
}

