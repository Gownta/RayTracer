#ifndef GOWNTA_ALGEBRA_H
#define GOWNTA_ALGEBRA_H

#include <iostream>
#include <cmath>

class Point3D;
class Vector3D;
class UnitVector3D;
class Colour;
class Matrix4x4;

///////////////////////////////////////////////////////////////////////////////
// RTriple

struct RTriple {
  RTriple() : v_() {}
  RTriple(double v1, double v2, double v3) : v_{v1,v2,v3} {}
  RTriple(double v[3]) : v_{v[0], v[1], v[2]} {}
  // other constructors are default

  double   operator[](std::size_t i) const { return v_[i]; }
  double & operator[](std::size_t i)       { return v_[i]; }

  RTriple sum  (const RTriple & o) const { return RTriple(v_[0]+o[0], v_[1]+o[1], v_[2]+o[2]); }
  RTriple diff (const RTriple & o) const { return RTriple(v_[0]-o[0], v_[1]-o[1], v_[2]-o[2]); }
  RTriple mult (const RTriple & o) const { return RTriple(v_[0]*o[0], v_[1]*o[1], v_[2]*o[2]); }
  RTriple scale(double s)          const { return RTriple(v_[0]*s,    v_[1]*s,    v_[2]*s); }
  RTriple neg  ()                  const { return RTriple(-v_[0],     -v_[1],     -v_[2]); }

  virtual char get_prefix() const { return '?'; }

protected:
  double v_[3];
};

#define GOWNTA_RTRIPLEC(Klass)                                      \
    Klass() : RTriple() {}                                          \
    Klass(double d1, double d2, double d3) : RTriple(d1, d2, d3) {} \
    Klass(double v[3]) : RTriple(v) {}                              \
    explicit Klass(const RTriple & rt) : RTriple(rt) {}

///////////////////////////////////////////////////////////////////////////////
// Point, Vector, UnitVector, Colour

struct Point3D : public RTriple {
  GOWNTA_RTRIPLEC(Point3D)
  virtual char get_prefix() const { return 'p'; }
};

struct Vector3D : public RTriple {
  GOWNTA_RTRIPLEC(Vector3D)

  double dot(const Vector3D & other) const
    { return v_[0]*other.v_[0] + v_[1]*other.v_[1] + v_[2]*other.v_[2]; }
  Vector3D cross(const Vector3D & other) const
    { return Vector3D(
            v_[1]*other.v_[2] - v_[2]*other.v_[1],
            v_[2]*other.v_[0] - v_[0]*other.v_[2],
            v_[0]*other.v_[1] - v_[1]*other.v_[0]); }

  double length2() const
    { return dot(*this); }
  double length() const
    { return sqrt(length2()); }

  double normalize();
  Vector3D unit() const { Vector3D ret = *this; ret.normalize(); return ret; }
  bool is_unit() const { return std::abs(length() - 1.0) < 1e-5; }

  virtual char get_prefix() const { return 'v'; }
};

struct UnitVector3D : public Vector3D {
  UnitVector3D(const Vector3D & v) : Vector3D(v) { normalize(); }
  virtual char get_prefix() const { return 'u'; }
};

struct Colour : public RTriple {
  GOWNTA_RTRIPLEC(Colour)
  double R() const { return v_[0]; }
  double G() const { return v_[1]; }
  double B() const { return v_[2]; }
  virtual char get_prefix() const { return 'c'; }
};

///////////////////////////////////////////////////////////////////////////////
// Operators

inline Vector3D operator+(const Vector3D & v1, const Vector3D & v2) { return Vector3D(v1.sum(v2)); }
inline Vector3D operator-(const Vector3D & v1, const Vector3D & v2) { return Vector3D(v1.diff(v2)); }
inline Vector3D operator*(double s, const Vector3D & v) { return Vector3D(v.scale(s)); }
inline Vector3D operator/(const Vector3D & v, double s) { return Vector3D(v.scale(1.0/s)); }
inline Vector3D operator-(const Vector3D & v) { return Vector3D(v.neg()); }

inline Point3D  operator+(const Point3D & p, const Vector3D & v) { return Point3D(p.sum(v)); }
inline Point3D  operator-(const Point3D & p, const Vector3D & v) { return Point3D(p.diff(v)); }

inline Vector3D operator-(const Point3D & p1, const Point3D & p2) { return Vector3D(p1.diff(p2)); }

inline double   dot  (const Vector3D & v1, const Vector3D & v2) { return v1.dot(v2); }
inline Vector3D cross(const Vector3D & v1, const Vector3D & v2) { return v1.cross(v2); }

inline Colour operator+(const Colour & c1, const Colour & c2) { return Colour(c1.sum(c2)); }
inline Colour operator-(const Colour & c1, const Colour & c2) { return Colour(c1.diff(c2)); }
inline Colour operator*(const Colour & c1, const Colour & c2) { return Colour(c1.mult(c2)); }
inline Colour operator*(double s, const Colour & c) { return Colour(c.scale(s)); }
inline Colour operator/(const Colour & c, double s) { return Colour(c.scale(1.0/s)); }

inline std::ostream & operator<<(std::ostream & os, const RTriple & rt)
{ return os << rt.get_prefix() << "<" << rt[0] << "," << rt[1] << "," << rt[2] << ">"; }

///////////////////////////////////////////////////////////////////////////////
// Matrix4x4

class Matrix4x4 {
public:
  Matrix4x4() : v_{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} {}

  Matrix4x4 transpose() const;
  Matrix4x4 invert() const;

  double & operator()(std::size_t r, std::size_t c)
  { return v_[4*r + c]; }
  double operator()(std::size_t r, std::size_t c) const
  { return v_[4*r + c]; }

  const double * operator[](std::size_t r) const { return v_ + 4*r; }
        double * operator[](std::size_t r)       { return v_ + 4*r; }

private:
  double v_[16];
};

///////////////////////////////////////////////////////////////////////////////
// Matrix Operators

inline Matrix4x4 operator *(const Matrix4x4 & a, const Matrix4x4 & b) {
  Matrix4x4 ret;

  for (std::size_t r = 0; r < 4; ++r) for (std::size_t c = 0; c < 4; ++c) {
    ret(r,c) = 0;
    for (int i = 0; i < 4; ++i) ret(r,c) += a(r,i)*b(i,c);
  }

  return ret;
}

inline Vector3D operator *(const Matrix4x4 & M, const Vector3D & v) {
  return Vector3D(
                  v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2],
                  v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2],
                  v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2]);
}

inline UnitVector3D operator*(const Matrix4x4 & M, const UnitVector3D & u) {
  return M * static_cast<const Vector3D &>(u);
}

inline Point3D operator *(const Matrix4x4 & M, const Point3D & p) {
  return Point3D(
                 p[0] * M[0][0] + p[1] * M[0][1] + p[2] * M[0][2] + M[0][3],
                 p[0] * M[1][0] + p[1] * M[1][1] + p[2] * M[1][2] + M[1][3],
                 p[0] * M[2][0] + p[1] * M[2][1] + p[2] * M[2][2] + M[2][3]);
}

std::ostream & operator<<(std::ostream & os, const Matrix4x4 & m);

///////////////////////////////////////////////////////////////////////////////

#endif // GOWNTA_ALGEBRA_H

