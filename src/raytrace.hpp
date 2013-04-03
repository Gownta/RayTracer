#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "scene.hpp"
#include "algebra.hpp"
#include <list>
#include "light.hpp"
#include "intersection.hpp"
#include "zpic.hpp"

void setup(SceneNode * root, const Point3D & eye,
           const Colour & ambient, const std::list<Light*> & lights,
           int width, int height, int x_min, int x_max, int y_min, int y_max,
           UnitVector3D X, UnitVector3D Y, UnitVector3D Z, double fov_scale
          );

void raytrace(ZPic & zimg);

//Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray, double index_of_refraction = 0.0);

#endif // RAYTRACE_H

