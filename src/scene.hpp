#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "intersection.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

class SceneNode {
public:
  typedef vector<SceneNode*> ChildList;

public:
  SceneNode(const string & name);
  virtual ~SceneNode();

  // cummulative transformation - takes into account all parents' transformations
  const Matrix4x4 & get_transform() const { return m_trans; }
  const Matrix4x4 & get_inverse() const { return m_invtrans; }
  const Matrix4x4 & get_inverse_transpose() const { return m_invtranspose; }
  Matrix4x4 get_local_transform() const;

  void add_child(SceneNode * child) {
    m_children.push_back(child);
    child->m_parent = this;
  }

  void rotate(char axis, double angle);
  void rotate(const Matrix4x4 & rot);
  void scale(const Vector3D & amount);
  void translate(const Vector3D & amount);

  const ChildList & children() const { return m_children; }
  ChildList & children() { return m_children; }

  const string & get_name() const { return m_name; }

  virtual void determine_bounds();

private:
  // labeling
  string m_name;

  // transformation
  Vector3D m_scale;
  Vector3D m_translate;
  Matrix4x4 m_rotate;

  void update_trans();
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;
  Matrix4x4 m_invtranspose;

  // structure
  SceneNode * m_parent;
  ChildList m_children;

protected:
  // boundary, before transformation
  double m_bounding_radius;
};

class JointNode : public SceneNode {
public:
  JointNode(const string & name) : SceneNode(name) {}

  void set_joint_x(double min, double init, double max) {}
  void set_joint_y(double min, double init, double max) {}

  struct JointRange {
    double min, init, max;
  };

protected:
  JointRange m_joint_x, m_joint_y;
};

///////////////////////////////////////////////////////////////////////////////

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name, Primitive* primitive)
    : SceneNode(name), m_material(0), m_primitive(primitive) {}

  const Material* get_material() const { return m_material; }
  Material* get_material() { return m_material; }

  void set_material(Material* material) { m_material = material; }

  Intersection intersect(const Point3D & origin, const Vector3D & ray);
  virtual void determine_bounds();

protected:
  Material* m_material;
  Primitive* m_primitive;
};

///////////////////////////////////////////////////////////////////////////////










































/*
class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

protected:
  Material* m_material;
  Primitive* m_primitive;
};
*/

#endif
