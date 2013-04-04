#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
struct Light {
  Light();
  
  Colour colour;
  Point3D position;
  double falloff[3];

  double intensity(double d) const { return 1 / (falloff[0] + d*falloff[1] + d*d*falloff[2]); }
  double intensity(const Point3D & p) const { return intensity((position - p).length()); }
};

std::ostream& operator<<(std::ostream& out, const Light& l);

#endif
