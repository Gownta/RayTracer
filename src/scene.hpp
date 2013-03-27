#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "algorithms.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Node Base Class

class SceneNode {
public:
  // typdefs
  typedef vector<SceneNode*> ChildList;

  // construction
  SceneNode(const string & name);
  virtual ~SceneNode();

  // name
  const string & get_name() const { return m_name; }

  // transformations
  const Matrix4x4 & get_trans()     const { return m_trans; }
  const Matrix4x4 & get_inverse()   const { return m_inverse; }
  const Matrix4x4 & get_normtrans() const { return m_normtrans; }

  void rotate(char axis, double angle);
  void rotate(const Matrix4x4 & rot);
  void scale(const Vector3D & amount);
  void translate(const Vector3D & amount);

  // children
  void add_child(SceneNode * child) { m_children.push_back(child); }

  const ChildList & children() const { return m_children; }
  ChildList & children() { return m_children; }

  // intersection
  int intersect(const Point3D & origin, const Vector3D & ray,
                IntersectionMode mode, Intersection where[]) const;

  // bounds
  virtual void determine_bounds();
  void set_bounds(const BoundingSphere & bs) { m_bounds = bs; }

private:
  string m_name;

  Matrix4x4 m_trans;
  Matrix4x4 m_inverse;
  Matrix4x4 m_normtrans;
  void update_trans(const Matrix4x4 & t);

  ChildList m_children;

  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;

  BoundingSphere m_bounds;
};

///////////////////////////////////////////////////////////////////////////////
// Geometry Node

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name, Primitive* primitive)
    : SceneNode(name), m_material(0), m_primitive(primitive)/*, m_bounds()*/ {}

  const Material* get_material() const { return m_material; }
  Material* get_material() { return m_material; }

  void set_material(Material* material) { m_material = material; }

  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;
  virtual void determine_bounds();

protected:
  Material* m_material;
  Primitive* m_primitive;
};

///////////////////////////////////////////////////////////////////////////////
// CSG Combiner

class CSGNode : public SceneNode {
public:
  enum Op {
    UNION,
    INTERSECTION,
    DIFFERENCE
  };

  CSGNode(const std::string & name, SceneNode & p1, Op op, SceneNode & p2, double br)
      : SceneNode(name), m_p1(p1), m_op(op), m_p2(p2) 
   {
    BoundingSphere bs;
    bs.origin = Point3D(0,0,0);
    bs.radius = (br ? br : INFINITY);
    set_bounds(bs);
  }
 
  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;
  virtual void determine_bounds();

  bool in(bool in1, bool in2) const {
    switch (m_op) {
    case UNION:        return in1 ||  in2;
    case INTERSECTION: return in1 &&  in2;
    case DIFFERENCE:   return in1 && !in2;
    }
    assert(false);
  }

private:
  SceneNode & m_p1;
  Op m_op;
  SceneNode & m_p2;
};

#endif

