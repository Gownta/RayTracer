#include "antialias.hpp"
#include "program_options.hpp"

using namespace std;

static double colour_var(const Colour & c1, const Colour & c2) {
  double ret = 0;
  for (int i = 0; i < 3; ++i) {
    double delta = c1[i] - c2[i];
    ret += delta * delta;
  }
  return ret;
}

bool aliasing(const ZPic & zimg, int x, int y, int x_min, int x_max, int y_min, int y_max) {
  // variance
  double var = 0;
  double weight = 0;
  double cutoff = cmd_options()["anti-aliasing"].as<double>();
  
  // cross variance
  {
    if (x - 1 >= x_min) var += colour_var(zimg.pixel(x,y), zimg.pixel(x-1,y));
    if (x + 1 <  x_max) var += colour_var(zimg.pixel(x,y), zimg.pixel(x+1,y));
    if (y - 1 >= y_min) var += colour_var(zimg.pixel(x,y), zimg.pixel(x,y-1));
    if (y + 1 <  y_max) var += colour_var(zimg.pixel(x,y), zimg.pixel(x,y+1));
    weight += 4;
  }

  return (var / weight) > cutoff;
}

