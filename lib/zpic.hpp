#ifndef GOWNTA_ZPIC_H
#define GOWNTA_ZPIC_H

#include "algebra.hpp"
#include "image.hpp"

struct ZPic {
  struct Z {
    Colour colour;
    Colour back;
    double alpha;
    double depth;
  };

  ZPic(int width, int height);
  ~ZPic() { delete[] m_z; }

  ZPic(const ZPic &) = delete; // implement if needed
  ZPic & operator=(const ZPic &) = delete; // implement if needed

  void read(const char * filename);
  bool write(const char * filename) const;
  
  void write(Image & img) const;

  const Z & operator()(int i, int j) const { return m_z[i*m_h + j]; }
        Z & operator()(int i, int j)       { return m_z[i*m_h + j]; }

  Colour pixel(int i, int j) const;

  int height() const { return m_h; }
  int width()  const { return m_w; }

private:
  int m_h, m_w;
  Z * m_z;
};

#endif // GOWNTA_ZPIC

