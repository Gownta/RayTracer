#ifndef INTERSECT_H
#define INTERSECT_H

#include "material.hpp"
#include "algebra.hpp"

static const double EPSILON = 1e-10;

struct Intersection {
  double distance;
  Material * material;
  Vector3D normal;
  double texture_pos[2];

  Intersection() : distance(INFINITY), material(NULL), normal(), texture_pos() {}

  operator bool() const { return distance != INFINITY; }
  bool operator<(const Intersection & other) const { return distance < other.distance; }
};

struct Intersection2 {
  double distance;
  Colour colour;

  Intersection2() : distance(INFINITY) {}

  operator bool() const { return distance != INFINITY; }
  bool operator<(const Intersection2 & other) const { return distance < other.distance; }
};

#endif  // INTERSECT_H

