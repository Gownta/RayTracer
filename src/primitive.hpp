#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "intersection.hpp"
#include <vector>
#include <iosfwd>

#include <cassert>
#include <algorithm>

class Primitive {
public:
  virtual ~Primitive() {}
  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray) 
    // = 0;
    { return Intersection(); }
  virtual double get_bounding_radius() const { return 0; }
};

/*class Sphere : public Primitive {
public:
  virtual ~Sphere() {}
};

class Cube : public Primitive {
public:
  virtual ~Cube() {}
};*/

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius) {}
  virtual ~NonhierSphere() {}

  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray);
  virtual double get_bounding_radius() const;

private:
  Point3D m_pos;
  double m_radius;
};

class Sphere : public NonhierSphere {
public:
  Sphere() : NonhierSphere(Point3D(0,0,0), 1) {}
};


// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces)
    : m_verts(verts), m_faces(faces) {}

  typedef std::vector<int> Face;

  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray);
  virtual double get_bounding_radius() const;
  
protected:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

class NonhierBox : public Mesh {
public:
  NonhierBox(const Point3D& pos, double size)
      : Mesh(std::vector<Point3D>(), std::vector<Face>()) {
    Vector3D x(size/2,0,0);
    Vector3D y(0,size/2,0);
    Vector3D z(0,0,size/2);
    for (int i = 0; i < 8; ++i) {
      Point3D place = pos + x + y + z;
      if (i & 0x1) place = place + x;
      else         place = place - x;
      if (i & 0x2) place = place + y;
      else         place = place - y;
      if (i & 0x4) place = place + z;
      else         place = place - z;
      m_verts.push_back(place);
    }
    
    for (int j = 0; j < 6; ++j) {
      Face f;
      for (int i = 0; i < 8; ++i) {
        bool exclude = 
          (j == 0 &&  (i & 0x1)) ||
          (j == 1 && !(i & 0x1)) ||
          (j == 2 &&  (i & 0x2)) ||
          (j == 3 && !(i & 0x2)) ||
          (j == 4 &&  (i & 0x4)) ||
          (j == 5 && !(i & 0x4));
        if (!exclude) f.push_back(i);
      }
      
      // problem - we need to order the vertices to be around the face, not diagonally.
      if ((m_verts[f[0]] - m_verts[f[1]]).length2() > 1) std::swap(f[0], f[3]);
      if ((m_verts[f[1]] - m_verts[f[2]]).length2() > 1) std::swap(f[0], f[1]);

      assert(f.size() == 4);
      m_faces.push_back(f);
    }
  }
  
  virtual ~NonhierBox() {}
};

class Cube : public NonhierBox {
public:
  Cube() : NonhierBox(Point3D(0,0,0), 1) {}
};

/*
class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox() {}

  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray);

private:
  Point3D m_pos;
  double m_size;
};
*/

#endif
