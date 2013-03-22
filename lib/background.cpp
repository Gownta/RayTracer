#include "background.hpp"
#include "program_options.hpp"

void set_background(Image & img) {
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

      img(x, y, 0) = (double)red / 2;
      img(x, y, 1) = 0;
      img(x, y, 2) = (double)(1 - red) / 2;
    }
  } else if (bg == 2) {
    // blue gradient
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y, 0) = 0;
      img(x, y, 1) = 0;
      img(x, y, 2) = (double)y / height;
    }
  } else if (bg == 3) {
    // reverse sky gradient
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y, 0) = 1 - (double)y / height;
      img(x, y, 1) = 1 - (double)y / height;
      img(x, y, 2) = 1;
    }
  } else {
    // black
    for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
      img(x, y, 0) = 0;
      img(x, y, 1) = 0;
      img(x, y, 2) = 0;
    }
  }
}

