#ifndef GOWNTA_ANTIALIAS_H
#define GOWNTA_ANTIALIAS_H

#include "zpic.hpp"
#include <vector>

class AntiAliaser {
public:
  AntiAliaser(ZPic & zimg, int x_min, int x_max, int y_min, int y_max);
  void antialias();

private:
  struct VW {
    VW(int dx, int dy, double weight) : dx(dx), dy(dy), weight(weight) {}
    int dx, dy;
    double weight;
  };

  double colour_var(const Colour & c1, const Colour & c2) const;
  bool needs_aliasing(int x, int y) const;

private:
  ZPic & m_zimg;

  int m_x_min, m_x_max;
  int m_y_min, m_y_max;

  std::vector<VW> m_aliasing;
  bool m_max;
  double m_cutoff;
};

//void antialias(ZPic & zimg, int x_min, int x_max, int y_min, int y_max);
//bool aliasing(const ZPic & zimg, int x, int y, int x_min, int x_max, int y_min, int y_max);

#endif // GOWNTA_ANTIALIAS_H

