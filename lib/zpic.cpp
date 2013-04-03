#include "zpic.hpp"
#include <cassert>
#include <iostream>

using namespace std;

ZPic::ZPic(int width, int height, int depth)
    : m_h(height)
    , m_w(width)
    , m_d(depth)
    , m_z(new Z[height*width*depth])
{
  assert(m_z);
  ZPic & self = *this;
  for (int i = 0; i < width; ++i) for (int j = 0; j < height; ++j) for (int k = 0; k < depth; ++k) {
    self(i,j,k).depth = INFINITY;
    self(i,j,k).back = Colour(1,1,1);
  }
}

Colour ZPic::pixel(int i, int j) const {
  const ZPic & self = *this;
  Colour ret(0,0,0);
  for (int k = 0; k < depth(); ++k) {
    double alpha;
    if (self(i,j,k).depth == INFINITY) alpha = 0;
    else                               alpha = self(i,j,k).alpha;
    assert(alpha >= 0 && alpha <= 1);
    ret = ret + alpha * self(i,j,k).colour + (1 - alpha) * self(i,j,k).back;
  }
  return (1.0 / depth()) * ret;
  /*
  const ZPic & self = *this;
  double alpha;
  if (self(i,j).depth == INFINITY) alpha = 0;
  else                             alpha = self(i,j).alpha;
  if (alpha < 0 && alpha > 1) {
    cout << "alpha = " << alpha << endl;
    assert(false);
  }
  return alpha * self(i, j).colour + (1 - alpha) * self(i, j).back;
  */
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

