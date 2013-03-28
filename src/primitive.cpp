#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>
#include "polyroots.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Algebraics

Algebraic::Algebraic(const string & eqn, double radius)
    : m_eqn(parse_equation(eqn, true))
    , m_xyzq(parse_equation(eqn, false))
    , m_deg(0)
    , m_br(radius)
{
  // determine the algebraic's degree
  for (const auto & f : m_eqn) {
    m_deg = max(m_deg, f.pow_t);
  }

  // parse the equation without expanding w -> (o_x + t * r_w)
  // use this equation to compute the partial derivatives
  auto unex = parse_equation(eqn, false);
  for (const auto & f : unex) {
    if (f.pow_x > 0) {
      m_dx.push_back(f);
      m_dx.back().k *= f.pow_x;
      m_dx.back().pow_x--;
    }
    if (f.pow_y > 0) {
      m_dy.push_back(f);
      m_dy.back().k *= f.pow_y;
      m_dy.back().pow_y--;
    }
    if (f.pow_z > 0) {
      m_dz.push_back(f);
      m_dz.back().k *= f.pow_z;
      m_dz.back().pow_z--;
    }
  }
}

int Algebraic::intersections(const Point3D & origin, const Vector3D & ray,
                             IntersectionMode mode, Intersection where[]) const {
  /*// for numerical stability, bring the origin closer to the bounding volume
  // add inct to distance before returning the point of intersection
  double inct = (Vector3D(_origin).length() - m_br) / ray.length();
  if (inct < 0) inct = 0;
  Point3D origin = _origin + inct*ray;*/

  // compute the intersection polynomial
  double c[5] = { 0 };
  for (const auto & f : m_eqn) {
    c[f.pow_t] += f.k
                  * pow(origin[0], f.pow_ox) 
                  * pow(origin[1], f.pow_oy) 
                  * pow(origin[2], f.pow_oz) 
                  * pow(ray[0], f.pow_rx)
                  * pow(ray[1], f.pow_ry)
                  * pow(ray[2], f.pow_rz);
  }

  // find the roots
  double roots[4];
  size_t num_roots;

  switch (m_deg) {
    case 4: num_roots = quarticRoots  (c[3] / c[4], c[2]/c[4], c[1]/c[4], c[0]/c[4], roots); break;
    case 3: num_roots = cubicRoots    (c[2] / c[3], c[1]/c[3], c[0]/c[3], roots); break;
    case 2: num_roots = quadraticRoots(c[2], c[1], c[0], roots); break;
    //case 1: num_roots = 1; roots[0] = -c[0] / c[1]; break;
    default: 
      cout << "\n" << m_deg << endl;
      assert(false);
  }

  /////////////////////////////////////  
  // return the roots
  
  if (mode == EXISTENCE) return num_roots > 0;

  double closest = INFINITY;
  int ret = 0;

  for (size_t i = 0; i < num_roots; ++i) {
    double root = roots[i];
    if (root > 1e-5 && (mode == ALL || root < closest)) {
      // compute the normal
      Point3D at  = origin + root * ray;
      double dx = 0, dy = 0, dz = 0;
      for (const auto & f : m_dx) {
        dx += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      for (const auto & f : m_dy) {
        dy += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      for (const auto & f : m_dz) {
        dz += f.k
              * pow(at[0], f.pow_x)
              * pow(at[1], f.pow_y)
              * pow(at[2], f.pow_z);
      }
      Vector3D normal = Vector3D(dx, dy, dz).unit();

      double dot = normal.dot(ray);
      if (dot > 0) normal = -normal;
      
      // add this intersection point
      where[ret].distance = root ;//+ inct;
      where[ret].normal = normal;
      ret++;

      //closest = min(closest, root);
      assert(mode != CLOSEST || root < closest);
      closest = root;
    }
  }

  if (mode == ALL) return ret;
  
  // mode == CLOSEST
  if (ret == 0) return ret;
  if (ret != 1) where[0] = where[ret - 1];
  return 1;
}

BoundingSphere Algebraic::get_bounds() const {
  BoundingSphere ret;
  ret.origin = Point3D(0,0,0);
  ret.radius = m_br + 1e-5;
  return ret;
}

bool Algebraic::contains(const Point3D & p) const {
  double val = 0;
  for (const auto & f : m_xyzq) {
    double t = f.k
               * pow(p[0], f.pow_x)
               * pow(p[1], f.pow_y)
               * pow(p[2], f.pow_z);
    val += t;
  }
  return val <= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Meshes

int Mesh::intersections(const Point3D & origin, const Vector3D & uray,
                        IntersectionMode mode, Intersection where[]) const {
  double closest = INFINITY;
  int ret = 0;
  for (vector<Face>::const_iterator it = m_faces.begin(); it != m_faces.end(); ++it) {
    // does the ray intersect this face?
    const Face & face = *it;
    
    // get a normal for the face
    Vector3D seg1 = m_verts[face.at(1)] - m_verts[face.at(0)];
    Vector3D seg2 = m_verts[face.at(2)] - m_verts[face.at(1)];
    Vector3D normal = seg1.cross(seg2).unit();
    
    double dot = uray.dot(normal);
    if (dot > 0) {
      normal = -normal;
      dot = -dot;
    }

    // a point is on the plane of the face if (p - q) dot n == 0
    // a point is on the ray if p = o + tr
    // solve
    if (-dot < EPSILON) continue;
    double t = (origin - m_verts[face.at(0)]).dot(normal) / -dot;

    // TODO numerically unstable - |o+tr dot normal| might exceed EPSILON
    
    if (t > EPSILON && (t < closest || mode != CLOSEST)) {
      // before we update the closest point of intersection, we need to make sure that the point is inside the poly
      // TODO: OPTIMIZE

      Point3D point = origin + t * uray;
      Vector3D expected_cross = 
          (m_verts[face[0]] - m_verts[face.back()])
          .cross
          (point            - m_verts[face.back()]);

      bool inside = true;
      for (size_t i = 1; i < face.size(); ++i) {
        Point3D v0 = m_verts[face[i-1]];
        Point3D v1 = m_verts[face[i]];

        Vector3D edge = v1    - v0;
        Vector3D dir  = point - v0;

        if (edge.cross(dir).dot(expected_cross) <= 0) {
          inside = false;
          break;
        }
      }

      if (inside) {
        if (mode == EXISTENCE) return 1;

        where[ret].distance = t;
        where[ret].normal = normal;
        where[ret].texture_pos[0] = (point[0] + 1) / 2.0;
        where[ret].texture_pos[1] = (point[2] + 1) / 2.0;
        ret++;

        assert(mode != CLOSEST || t < closest);
        closest = t;
      }
    }
  }

  if (mode == ALL) return ret;
  
  // mode == CLOSEST
  if (ret == 0) return ret;
  if (ret != 1) where[0] = where[ret - 1];
  return 1;
}

BoundingSphere Mesh::get_bounds() const {
  return find_bounding_sphere(m_verts);
}

///////////////////////////////////////////////////////////////////////////////
// Instatiations

Cube::Cube() : Mesh(vector<Point3D>(), vector<Face>()) {
  for (int i = 0; i < 8; ++i) {
    Point3D corner((bool)(i & 0x1), (bool)(i & 0x2), (bool)(i & 0x4));
    m_verts.push_back(corner);
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
    
    // we need to order the vertices to be around the face, not diagonally.
    if ((m_verts[f[0]] - m_verts[f[1]]).length2() > 1) std::swap(f[0], f[3]);
    if ((m_verts[f[1]] - m_verts[f[2]]).length2() > 1) std::swap(f[0], f[1]);

    assert(f.size() == 4);
    m_faces.push_back(f);
  }
}

///////////////////////////////////////////////////////////////////////////////
// printing

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}

