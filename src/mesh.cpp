#include "mesh.hpp"
#include <iostream>
#include <cassert>
#include "intersection.hpp"

using namespace std;

/*Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces) {}*/

Intersection Mesh::intersect(const Point3D & origin, const Vector3D & uray) {
  // TODO: OPTIMIZE
  Intersection closest;

  for (vector<Face>::iterator it = m_faces.begin(); it != m_faces.end(); ++it) {
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

    // TODO numerically unstable
    /*// due to numerical instability on high angles, we need some refinement
    if (-dot < 1e-3) {
      
    }

    // due to numerical instability, we want to run the calculation again, this time using o+tr as the origin, then accumulate
    Point3D approx = origin + (t - 1) * uray;
    double t2 = (approx - m_verts[face.at(0)]).dot(normal) / -dot;
    t += (t2 - 1);

    double off = 
    //assert(
    abs(((origin + t * uray) - m_verts[face[0]]).dot(normal)) ;//< EPSILON);
    if (off > EPSILON) {
      cout << "error of " << off << "\n";
      cout << "dot was  " << dot << "\n";
      exit(1);
    }*/
    
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
      }
/*
      bool ander = 1;
      bool orer  = 0;
      for (size_t i = 0; i < face.size(); ++i) {
        size_t j = i + 1;
        if (j == face.size()) j = 0;

        Point3D vi = m_verts[face[i]];
        Point3D vj = m_verts[face[j]];

        Vector3D edge = vj    - vi;
        Vector3D dir  = point - vi;

        bool side = edge.cross(dir).length2() > 0;
        if (edge.cross(dir).length2() > EPSILON) {
          ander = ander && side;
          orer  = orer  || side;
        }
      }

      if (ander == 1 || orer == 0) {
        closest.distance = t;
        closest.normal = normal;
      }
      */
      /*

      Vector3D edge1 = m_verts[face[1]] - m_verts[face[0]];
      Vector3D edge2 = m_verts[face[2]] - m_verts[face[1]];
      Vector3D edge3 = m_verts[face[0]] - m_verts[face[2]];
      Vector3D dir1  = point - m_verts[face[0]];
      Vector3D dir2  = point - m_verts[face[1]];
      Vector3D dir3  = point - m_verts[face[2]];

      cout
      << "Mesh Intersection: point = " << point << "\n"
      << "                   v1    = " << m_verts[face[0]] << "\n"
      << "                   v2    = " << m_verts[face[1]] << "\n"
      << "                   v3    = " << m_verts[face[2]] << "\n"
      << "                   v2-v1 = " << edge1 << "\n"
      << "                   v3-v2 = " << edge2 << "\n"
      << "                   v1-v3 = " << edge3 << "\n"
      << "                   p-v1  = " << dir1 << "\n"
      << "                   p-v2  = " << dir2 << "\n"
      << "                   p-v3  = " << dir3 << "\n"
      << "                   d1    = " << (edge1.dot(dir1) > 0) << "\n"
      << "                   d2    = " << (edge2.dot(dir2) > 0) << "\n"
      << "                   d3    = " << (edge3.dot(dir3) > 0) << "\n"
      << "                   c1    = " << (edge1.cross(dir1) > 0) << "\n"
      << "                   c2    = " << (edge2.cross(dir2) > 0) << "\n"
      << "                   c3    = " << (edge3.cross(dir3) > 0) << "\n"
      << endl;
      }*/
    }
  }

  return closest;
}

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
