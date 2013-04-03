#ifndef GOWNTA_ZPIC_H
#define GOWNTA_ZPIC_H

#include "algebra.hpp"
#include "image.hpp"

struct ZPic {
  struct Z {
    Colour colour;
    Colour back;
    double alpha;
    double depth;  // set to zero if pixel shouldn't be raytraced
    double dx, dy; // offset of ray inside pixel
  };

  ZPic(int width, int height, int depth = 1);
  ~ZPic() { delete[] m_z; }

  ZPic(const ZPic &) = delete; // implement if needed
  ZPic & operator=(const ZPic &) = delete; // implement if needed

  void write(Image & img) const;

  const Z & operator()(int i, int j, int k = 0) const { return m_z[(i*m_h + j)*m_d + k]; }
        Z & operator()(int i, int j, int k = 0)       { return m_z[(i*m_h + j)*m_d + k]; }

  Colour pixel(int i, int j) const;

  int height() const { return m_h; }
  int width()  const { return m_w; }
  int depth()  const { return m_d; }

private:
  int m_h, m_w, m_d;
  Z * m_z;
};

#endif // GOWNTA_ZPIC

