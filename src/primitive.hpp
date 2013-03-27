#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "intersection.hpp"
#include "algorithms.hpp"
#include <vector>
#include <iosfwd>

#include <cassert>
#include <algorithm>
#include "calc.hpp"

///////////////////////////////////////////////////////////////////////////////
// Primitive Base-Class

class Primitive {
public:
  virtual ~Primitive() {}

  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const = 0;
  virtual BoundingSphere get_bounds() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Algebraics

class Algebraic : public Primitive {
public:
  Algebraic(const string & eqn, double radius);
  
  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;
  virtual BoundingSphere get_bounds() const;

private:
  vector<Factor> m_eqn;
  vector<Factor> m_dx, m_dy, m_dz;
  int m_deg;
  double m_br;
};

///////////////////////////////////////////////////////////////////////////////
// Meshes

class Mesh : public Primitive {
public:
  typedef std::vector<int> Face;

  Mesh(const std::vector<Point3D>& verts,
       const std::vector<Face>& faces)
    : m_verts(verts), m_faces(faces) {}

  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;
  virtual BoundingSphere get_bounds() const;
  
protected:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

///////////////////////////////////////////////////////////////////////////////
// Specific Shapes

class Sphere : public Algebraic {
public:
  Sphere() : Algebraic("x^2 + y^2 + z^2 - 1", 1) {}
};

class Cube : public Mesh {
public:
  Cube();
};

#endif

