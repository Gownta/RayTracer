#include "zpic.hpp"
#include <cassert>

ZPic::ZPic(int width, int height)
    : m_h(height)
    , m_w(width)
    , m_z(new Z[height*width]) 
{
  ZPic & self = *this;
  for (int i = 0; i < width; ++i) for (int j = 0; j < height; ++j) {
    self(i,j).depth = INFINITY;
    self(i,j).back = Colour(0,0,0);
  }
}

Colour ZPic::pixel(int i, int j) const {
  const ZPic & self = *this;
  double alpha;
  if (self(i,j).depth == INFINITY) alpha = 0;
  else                             alpha = self(i,j).alpha;
  assert(alpha >= 0 && alpha <= 1);
  return alpha * self(i, j).colour + (1 - alpha) * self(i, j).back;
}

void ZPic::write(Image & img) const {
  assert(width()  == img.width());
  assert(height() == img.height());
  assert(img.elements() == 3);

  for (int i = 0; i < width(); ++i) for (int j = 0; j < height(); ++j) {
    Colour put = pixel(i, j);
    for (int k = 0; k < 3; ++k) img(i,j,k) = put[k];
  }
}

