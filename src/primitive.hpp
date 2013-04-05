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

  bool contains(const Point3D & p) const;

private:
  vector<Factor> m_eqn;
  vector<Factor> m_xyzq;
  vector<Factor> m_dx, m_dy, m_dz;
  int m_deg;
  double m_br;
};

///////////////////////////////////////////////////////////////////////////////
// Meshes

class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector<std::vector<int>>& simple_faces);
  Mesh(const std::vector<Point3D>  & vertices,
       const std::vector<Vector3D> & normals,
       const std::vector<Point3D>  & textures,
       const std::vector<std::vector<std::vector<int>>> & faces); 

  virtual int intersections(const Point3D & origin, const Vector3D & ray,
                            IntersectionMode mode, Intersection where[]) const;
  virtual BoundingSphere get_bounds() const;
  
protected:
  struct Face {
    std::vector<int> vertex_indices;
    std::vector<int> normal_indices;
    std::vector<int> texture_indices;
    Point3D  base;
    Vector3D basic_normal;
  };
  std::vector<Point3D>  m_vertices;
  std::vector<Vector3D> m_normals;
  std::vector<Point3D>  m_textures;
  std::vector<Face>     m_faces;

  void basify_faces();

  //friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
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

