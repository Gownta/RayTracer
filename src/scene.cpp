#include "scene.hpp"
#include <iostream>
#include "a2.hpp"

SceneNode::SceneNode(const string & name)
  : m_name(name)
  , m_scale()
  , m_translate()
  , m_rotate()
  , m_trans()
  , m_invtrans()
  , m_invtranspose()
  , m_parent(0)
  , m_children()
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

void SceneNode::update_trans() {
  m_trans = translation(m_translate) * m_rotate * scaling(m_scale);
  if (m_parent != NULL) m_trans = m_parent->get_transform() * m_trans;
  m_invtrans = m_trans.invert();
  m_invtranspose = m_invtranspose.transpose();
}


Intersection GeometryNode::intersect(const Point3D & _origin, const Vector3D & _ray) {
  // perform the inverse transformation to get the ray from the primitives' perspective
  Point3D  origin = get_inverse() * _origin;
  Vector3D ray    = get_inverse() * _ray;

  // intersect with the primitives
  // for now, just one primitive
  Intersection result = m_primitive->intersect(origin, ray);

  // need to set the material and adjust the normal
  if (result) {
    result.material = m_material;
    result.normal = get_inverse_transpose() * result.normal;

    if (false)
    cout
    << "Intersection details: _origin = " << _origin << "\n"
    << "                      _ray    = " << _ray << "\n"
    << "                      dorigin = " << (origin - _origin) << "\n"
    << "                      dray    = " << (ray - _ray) << "\n"
    << "                      POI     = " << (_origin + result.distance * _ray) << "\n"
    << "                      R       = " << (_origin + result.distance * _ray - Point3D(200,50,-100)).length() << "\n"
    << "                      normal  = " << result.normal << "\n"
    << "                      inline? = " << result.normal.dot(_origin + result.distance * _ray - Point3D(200,50,-100)) << "\n"
    << endl;

    return result;
  }

  return Intersection();
}




































/*

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
  // Fill me in
}

void SceneNode::scale(const Vector3D& amount)
{
  std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
  // Fill me in
}

void SceneNode::translate(const Vector3D& amount)
{
  std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
  // Fill me in
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}
 */

