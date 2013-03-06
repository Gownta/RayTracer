#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "intersection.hpp"

class Primitive {
public:
  virtual ~Primitive() {}
  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray) 
    // = 0;
    { return Intersection(); }
};

class Sphere : public Primitive {
public:
  virtual ~Sphere() {}
};

class Cube : public Primitive {
public:
  virtual ~Cube() {}
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius) {}
  virtual ~NonhierSphere() {}

  virtual Intersection intersect(const Point3D & origin, const Vector3D & ray);

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox() {}

private:
  Point3D m_pos;
  double m_size;
};

#endif
