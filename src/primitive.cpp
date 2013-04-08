#include "primitive.hpp"
#include <cmath>
#include <cassert>
#include <iostream>
#include "polyroots.hpp"

using namespace std;

static inline double ipow(double b, int e) {
  switch (e) {
    case 0: return 1;
    case 1: return b;
    case 2: return b*b;
    case 3: return b*b*b;
    case 4: return (b*b)*(b*b);
  }
  assert(false);
}

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
  // compute the intersection polynomial
  double c[5] = { 0 };
  for (const auto & f : m_eqn) {
    c[f.pow_t] += f.k
                  * ipow(origin[0], f.pow_ox) 
                  * ipow(origin[1], f.pow_oy) 
                  * ipow(origin[2], f.pow_oz) 
                  * ipow(ray[0], f.pow_rx)
                  * ipow(ray[1], f.pow_ry)
                  * ipow(ray[2], f.pow_rz);
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
              * ipow(at[0], f.pow_x)
              * ipow(at[1], f.pow_y)
              * ipow(at[2], f.pow_z);
      }
      for (const auto & f : m_dy) {
        dy += f.k
              * ipow(at[0], f.pow_x)
              * ipow(at[1], f.pow_y)
              * ipow(at[2], f.pow_z);
      }
      for (const auto & f : m_dz) {
        dz += f.k
              * ipow(at[0], f.pow_x)
              * ipow(at[1], f.pow_y)
              * ipow(at[2], f.pow_z);
      }
      Vector3D normal = Vector3D(dx, dy, dz).unit();

      double dot = normal.dot(ray);
      if (dot > 0) normal = -normal;
      
      // add this intersection point
      where[ret].distance = root ;//+ inct;
      where[ret].normal = normal;
      ret++;

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
               * ipow(p[0], f.pow_x)
               * ipow(p[1], f.pow_y)
               * ipow(p[2], f.pow_z);
    val += t;
  }
  return val <= 0;
}

///////////////////////////////////////////////////////////////////////////////
// Meshes

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector<std::vector<int>> & simple_faces)
    : m_vertices(verts)
    , m_normals()
    , m_textures()
{
  for (const auto & vi : simple_faces) {
    Face f;
    f.vertex_indices = vi;
    m_faces.push_back(f);
  }
  basify_faces();
}

Mesh::Mesh(const std::vector<Point3D>  & vertices,
           const std::vector<Vector3D> & normals,
           const std::vector<Point3D>  & textures,
           const std::vector<std::vector<std::vector<int>>> & faces)
    : m_vertices(vertices)
    , m_normals(normals)
    , m_textures(textures)
{
  for (const auto & vvi : faces) {
    Face f;
    assert(vvi.size() == 3);
    f.vertex_indices  = vvi[0];
    f.normal_indices  = vvi[1];
    f.texture_indices = vvi[2];
    m_faces.push_back(f);
  }

  // normalize the normals
  for (auto & normal : m_normals) {
    normal.normalize();
  }

  triangulate_faces();
  basify_faces();
}

void Mesh::basify_faces() {
  for (auto & face : m_faces) {
    assert(face.vertex_indices.size() >= 3);
    const Point3D & p0 = m_vertices.at(face.vertex_indices.at(0));
    const Point3D & p1 = m_vertices.at(face.vertex_indices.at(1));
    const Point3D & p2 = m_vertices.at(face.vertex_indices.at(2));

    face.base = p0;
    face.basic_normal = (p1 - p0).cross(p2 - p1).unit();

    // ensure all indices are within range
    for (const auto & vi : face.vertex_indices ) m_vertices.at(vi);
    for (const auto & vi : face.normal_indices ) m_normals .at(vi);
    for (const auto & vi : face.texture_indices) m_textures.at(vi);

    // ensure only triangles have augmented data (may extend to quads later)
    if (face.normal_indices.size() > 0 || face.texture_indices.size() > 0) {
      assert(face.vertex_indices .size() == 3);
      assert(face.normal_indices .size() == 3 || face.normal_indices. empty());
      assert(face.texture_indices.size() == 3 || face.texture_indices.empty());
    }
  }
}

void Mesh::triangulate_faces() {
  vector<Face> old_faces;
  old_faces.swap(m_faces);
  assert(m_faces.empty());

  // for each non-triangular face (assumed convex), triangulate using the midpoint
  for (auto & face : old_faces) {
    assert(face.vertex_indices.size() > 2);
    if (face.vertex_indices.size() == 3) {
      m_faces.push_back(move(face));
    } else {
      vector<Face> to_add;
      to_add.resize(face.vertex_indices.size());

      // vertices
      if (true) {
        Vector3D sum(0,0,0);
        for (int v : face.vertex_indices) sum = sum + Vector3D(m_vertices[v]);
        Point3D midpoint(sum / face.vertex_indices.size());
        m_vertices.push_back(midpoint);

        for (size_t i = 0; i < face.vertex_indices.size(); ++i) {
          size_t j = (i + 1) % face.vertex_indices.size();
          to_add[i].vertex_indices.push_back(face.vertex_indices[i]);
          to_add[i].vertex_indices.push_back(face.vertex_indices[j]);
          to_add[i].vertex_indices.push_back(m_vertices.size() - 1);
        }
      }

      // normals
      if (face.normal_indices.size() > 0) {
        assert(face.normal_indices.size() == face.vertex_indices.size());
        Vector3D sum(0,0,0);
        for (int v : face.normal_indices) sum = sum + m_normals[v];
        Vector3D midnorm(sum / face.normal_indices.size());
        m_normals.push_back(midnorm);

        for (size_t i = 0; i < face.normal_indices.size(); ++i) {
          size_t j = (i + 1) % face.normal_indices.size();
          to_add[i].normal_indices.push_back(face.normal_indices[i]);
          to_add[i].normal_indices.push_back(face.normal_indices[j]);
          to_add[i].normal_indices.push_back(m_normals.size() - 1);
        }
      }

      // textures
      if (face.texture_indices.size() > 0) {
        assert(face.texture_indices.size() == face.vertex_indices.size());
        Vector3D sum(0,0,0);
        for (int v : face.texture_indices) sum = sum + Vector3D(m_textures[v]);
        Point3D midtexture(sum / face.texture_indices.size());
        m_textures.push_back(midtexture);

        for (size_t i = 0; i < face.texture_indices.size(); ++i) {
          size_t j = (i + 1) % face.texture_indices.size();
          to_add[i].texture_indices.push_back(face.texture_indices[i]);
          to_add[i].texture_indices.push_back(face.texture_indices[j]);
          to_add[i].texture_indices.push_back(m_textures.size() - 1);
        }
      }

      // add
      m_faces.insert(m_faces.end(), to_add.begin(), to_add.end());
    }
  }
}

int Mesh::intersections(const Point3D & origin, const Vector3D & uray,
                        IntersectionMode mode, Intersection where[]) const {
  double closest = INFINITY;
  int ret = 0;

  //cout << "attempting intersection with " << m_faces.size() << " faces\n";

  for (const auto & face : m_faces) {
    // does the ray intersect this face?

    // a point is on the plane of the face if (p - q) dot n == 0
    // a point is on the ray if p = o + tr
    // solve (albeit numerically unstable)
    double dot = uray.dot(face.basic_normal);
    if (abs(dot) < EPSILON) continue;
    double t = (origin - face.base).dot(face.basic_normal) / -dot;

    if (t > EPSILON && (t < closest || mode != CLOSEST)) {
      // before we update the closest point of intersection, we need to make sure that the point is inside the poly
      Point3D point = origin + t * uray;
      Vector3D expected_cross = 
          (m_vertices[face.vertex_indices[0]] - m_vertices[face.vertex_indices.back()])
          .cross
          (point                              - m_vertices[face.vertex_indices.back()]);

      bool inside = true;
      for (size_t i = 1; i < face.vertex_indices.size(); ++i) {
        Point3D v0 = m_vertices[face.vertex_indices[i-1]];
        Point3D v1 = m_vertices[face.vertex_indices[i  ]];

        Vector3D edge = v1    - v0;
        Vector3D dir  = point - v0;

        if (edge.cross(dir).dot(expected_cross) <= 0) {
          inside = false;
          break;
        }
      }

      if (inside) {
        if (mode == EXISTENCE) return 1; // if close enough

        // if necessary, compute the barycentric coordinates
        Vector3D bary(0,0,0);
        if (!face.normal_indices.empty() || !face.texture_indices.empty()) {
          const Point3D & a = m_vertices[face.vertex_indices[0]];
          const Point3D & b = m_vertices[face.vertex_indices[1]];
          const Point3D & c = m_vertices[face.vertex_indices[2]];
          bary = barycentric(a, b, c, point);
        }

        // compute the normal
        if (face.normal_indices.empty()) {
          where[ret].normal = face.basic_normal;
        } else {
          const Vector3D & an = m_normals[face.normal_indices[0]];
          const Vector3D & bn = m_normals[face.normal_indices[1]];
          const Vector3D & cn = m_normals[face.normal_indices[2]];
          where[ret].normal = bary[0] * an + bary[1] * bn + bary[2] * cn; 
        }
        if (where[ret].normal.dot(uray) > 0) where[ret].normal = -where[ret].normal;

        // compute the texture position
        if (face.texture_indices.empty()) {
          // this code can probably be deleted
          where[ret].texture_pos[0] = 0;
          where[ret].texture_pos[1] = 0;
        } else {
          const Vector3D at = Vector3D(m_textures[face.texture_indices[0]]);
          const Vector3D bt = Vector3D(m_textures[face.texture_indices[1]]);
          const Vector3D ct = Vector3D(m_textures[face.texture_indices[2]]);
          Vector3D pos = bary[0] * at + bary[1] * bt + bary[2] * ct;
          where[ret].texture_pos[0] = pos[0];
          where[ret].texture_pos[1] = pos[1];
        }

        where[ret].distance = t;
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
  return find_bounding_sphere(m_vertices);
}

///////////////////////////////////////////////////////////////////////////////
// Instatiations

Cube::Cube() : Mesh(vector<Point3D>(), vector<vector<int>>()) {
  for (int i = 0; i < 8; ++i) {
    Point3D corner((bool)(i & 0x1), (bool)(i & 0x2), (bool)(i & 0x4));
    m_vertices.push_back(corner);
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
      if (!exclude) f.vertex_indices.push_back(i);
    }
    
    // we need to order the vertices to be around the face, not diagonally.
    if ((m_vertices[f.vertex_indices[0]] - m_vertices[f.vertex_indices[1]]).length2() > 1)
      std::swap(f.vertex_indices[0], f.vertex_indices[3]);
    if ((m_vertices[f.vertex_indices[1]] - m_vertices[f.vertex_indices[2]]).length2() > 1)
      std::swap(f.vertex_indices[0], f.vertex_indices[1]);

    assert(f.vertex_indices.size() == 4);
    m_faces.push_back(f);
  }

  basify_faces();
}

