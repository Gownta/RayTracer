#include "primitive.hpp"
#include <iostream>
#include <cassert>
#include "intersection.hpp"

using namespace std;

/*Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces) {}*/

Intersection Mesh::intersect(const Point3D & origin, const Vector3D & uray) const {
  // TODO: OPTIMIZE
  Intersection closest;

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
    
    if (t > EPSILON && t < closest.distance) {
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
        closest.distance = t;
        closest.normal = normal;
        closest.texture_pos[0] = (point[0] + 1) / 2.0;
        closest.texture_pos[1] = (point[2] + 1) / 2.0;
      }
    }
  }

  return closest;
}

BoundingSphere Mesh::get_bounds() const {
  return find_minimal_bounding_sphere(m_verts);
}

/*double Mesh::get_bounding_radius() const {
  double max_distance = 0;
  for (vector<Point3D>::const_iterator it = m_verts.begin(); it != m_verts.end(); ++it) {
    max_distance = max(max_distance, (*it - Point3D(0,0,0)).length());
  }
  return max_distance;
}*/

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
