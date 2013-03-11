#include "scene.hpp"
#include <iostream>
#include "a2.hpp"

SceneNode::SceneNode(const string & name)
  : m_name(name)
  , m_trans()
  , m_inverse()
  , m_normtrans()
  , m_parent(0)
  , m_children()
  , m_bounding_radius(INFINITY)
{}

SceneNode::~SceneNode() {}

void SceneNode::rotate(char axis, double angle) {
  m_trans = m_trans * rotation(angle * 2*M_PI / 360, axis);
  update_trans();
}

void SceneNode::rotate(const Matrix4x4 & rot) {
  m_trans = m_trans * rot;
  update_trans();
}

void SceneNode::scale(const Vector3D& amount) {
  m_trans = m_trans * scaling(amount);
  update_trans();
}

void SceneNode::translate(const Vector3D& amount) {
  m_trans = m_trans * translation(amount);
  update_trans();
}

void SceneNode::update_trans() {
  m_inverse = m_trans.invert();
  m_normtrans = m_inverse.transpose();
}

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

  if (0)
  cout << "Attempting an intersection with " << get_name() << "\n"
       << "                      _origin = " << _origin << "\n"
       << "                      origin  = " << origin << "\n"
       << "                      _ray    = " << _ray << "\n"
       << "                      ray     = " << ray << "\n"
       << endl;

  return closest;
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
  /*Point3D origin = get_global_inverse() * _origin;
  Vector3D ray   = get_global_inverse() * _ray;
  */
  Point3D origin = get_inverse() * _origin;
  Vector3D ray   = get_inverse() * _ray;

  // try to intersect with the bounding sphere first
  //double closest_s = ((origin - Point3D(0,0,0)) - ((origin - Point3D(0,0,0)).dot(ray) * ray)).length2();
  //if (closest_s > m_bounding_radius * m_bounding_radius) return Intersection();

  // intersect with the primitive
  Intersection result = m_primitive->intersect(origin, ray);

  // need to set the material and adjust the normal
  if (result) {
    result.material = m_material;
    //result.normal = (get_global_normtrans() * result.normal).unit();
    result.normal = (get_normtrans() * result.normal).unit();
    assert(result.normal.dot(_ray) < 0);
    return result;
  }

  return Intersection();
}

void GeometryNode::determine_bounds() {
  m_bounding_radius = m_primitive->get_bounding_radius();
}

