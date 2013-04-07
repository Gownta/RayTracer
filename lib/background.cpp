#include "background.hpp"
#include "program_options.hpp"

void set_background(ZPic & img) {
  int width = img.width();
  int height = img.height();

  int bg = cmd_options()["background"].as<int>();

  if (bg == 1) {
    // red-blue checkerboard
    const int len = 32;
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      int box_x = x / len;
      int box_y = y / len;
      
      bool red = (box_x + box_y) % 2;

      img(x, y).back = Colour((double)red / 2, 0, (double)(1 - red) / 2);
    }
  } else if (bg == 2) {
    // blue gradient
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y).back = Colour(0, 0, (double)y / height);
    }
  } else if (bg == 3) {
    // reverse sky gradient
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y).back = Colour(0, 1 - (double)y / height, 1 - (double)y / height);
    }
  } else if (bg == 4) {
    // black-yellow stripes
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      double c = ((x + y) / 32) % 2;
      img(x, y).back = Colour(c, c, 0);
    }
  } else if (bg == 5) {
    // white
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y).back = Colour(1, 1, 1);
    }
  } else {
    // black
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y).back = Colour(0, 0, 0);
    }
  }
}

