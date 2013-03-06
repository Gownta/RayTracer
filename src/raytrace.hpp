#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "scene.hpp"
#include "algebra.hpp"
#include <list>
#include "light.hpp"
#include "intersection.hpp"

void setup(SceneNode * root, const Colour & ambient, const std::list<Light*> & lights);
Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray);

#endif // RAYTRACE_H

