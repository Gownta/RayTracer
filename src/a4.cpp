#include "a4.hpp"
#include "image.hpp"
#include <cmath>
#include "raytrace.hpp"
#include <ctime>
#include "progress_bar.hpp"
#include "background.hpp"
#include "program_options.hpp"
#include "zpic.hpp"
#include "antialias.hpp"
#include <cstdlib>

using namespace std;

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  /////////////////////////////////////
  // Setup.

  // compute the left axis
  Vector3D left = up.cross(view);

  // normalize all axes
  const UnitVector3D X = left;
  const UnitVector3D Y = up;
  const UnitVector3D Z = view;

  // find the field of view scaling factor
  // this factor is over the width of the image
  double fov_scale = tan((fov/2) * 2*M_PI / 360) / (width / 2);

  // determine which range of the image to render
  int x_min = 0;
  int x_max = width;
  int y_min = 0;
  int y_max = height;
  if (cmd_options().count("x-range")) {
    vector<int> xr = cmd_options()["x-range"].as<vector<int>>();
    if (xr.size() >= 1) x_min = xr[0];
    if (xr.size() >= 2) x_max = xr[1];
  }
  if (cmd_options().count("y-range")) {
    vector<int> yr = cmd_options()["y-range"].as<vector<int>>();
    if (yr.size() >= 1) y_min = yr[0];
    if (yr.size() >= 2) y_max = yr[1];
  }
  //int pixels = (x_max - x_min)*(y_max - y_min);

  // Initialize the image.
  ZPic zimg(width, height);
  set_background(zimg);
  for (int x = x_min; x < x_max; ++x) for (int y = y_min; y < y_max; ++y) zimg(x,y).dx = zimg(x,y).dy = 0.5;

  /////////////////////////////////////
  // Raytracing.

  setup(root, eye, ambient, lights,
        width, height, x_min, x_max, y_min, y_max,
        X, Y, Z, fov_scale);

  raytrace(zimg);

  AntiAliaser dealias(zimg, x_min, x_max, y_min, y_max);
  dealias.antialias();


  /////////////////////////////////////
  // Anti-Aliasing

  /*srand(time(NULL));
  if (0 && !cmd_options().count("no-anti-aliasing")) {
    bool needs_aliasing[x_max - x_min][y_max - y_min];
    int aliasing_count = 0;
    for (int x = x_min; x < x_max; ++x) for (int y = y_min; y < y_max; ++y) {
      log_progress("anti-aliasing", (double)x / (x_max - x_min) / 2);
      bool na = aliasing(zimg, x, y, x_min, x_max, y_min, y_max);
      needs_aliasing[x - x_min][y - y_min] = na;
      aliasing_count += na;
    }

    wipe_line();
    cout << aliasing_count << " / " << ((x_max - x_min)*(y_max - y_min))
         << " pixels (" << ((int)((double)aliasing_count / pixels * 100)) << "%) need anti-aliasing.\n";

    bool red = cmd_options().count("show-anti-aliasing");
    int resolution = cmd_options()["alias-method"].as<int>();
    for (int x = x_min; x < x_max; ++x) for (int y = y_min; y < y_max; ++y) {
      log_progress("anti-aliasing", (double)x / (x_max - x_min) / 2 + 0.5);
      if (needs_aliasing[x - x_min][y - y_min]) {
        if (red) {
          zimg(x,y).colour = Colour(1,0,0);
        } else if (resolution == 3) {
          // random
          int R = 10;
          double iR = 1.0 / R;
          zimg(x, y).colour = Colour(0,0,0);
          zimg(x, y).alpha  = 0;
          for (int i = 0; i < R; ++i) {
            double cx = (double)width / 2.0  - (x + 0.5);
            double cy = (double)height / 2.0 - (y + 0.5);
            cx += ((rand() % 1024) / 1024.0 - 0.5);
            cy += ((rand() % 1024) / 1024.0 - 0.5);
            
            Vector3D ray = (Z + cx * fov_scale * X + cy * fov_scale * Y).unit();
            Intersection2 display = get_colour(root, eye, ray);

            if (display) {
              zimg(x, y).colour  = zimg(x, y).colour + iR * display.colour;
              zimg(x, y).alpha  += iR;
            }
          }
        } else if (resolution == 4) {
          // rotated grid
          int dim = 3;
          Matrix4x4 rot = rotation('z', atan(0.5));
          int rdim = sqrt(2) * dim + 1;
          zimg(x, y).colour = Colour(0,0,0);
          zimg(x, y).alpha  = 0;
          int count = 0;
          for (int i = 0; i <= rdim; ++i) for (int j = 0; j <= rdim; ++j) {
            Point3D urp((double)i / rdim - 0.5, (double)j / rdim - 0.5, 0);
            urp = rot * urp;
            double dx = urp[0] + 0.5; if (dx < 0 || dx > 1) continue;
            double dy = urp[1] + 0.5; if (dy < 0 || dy > 1) continue;
            count++;

            double cx = (double)width / 2.0  - x + dx;
            double cy = (double)height / 2.0 - y + dy;

            Vector3D ray = (Z + cx * fov_scale * X + cy * fov_scale * Y).unit();
            Intersection2 display = get_colour(root, eye, ray);

            if (display) {
              zimg(x, y).colour  = zimg(x, y).colour + display.colour;
              zimg(x, y).alpha  += 1;
            }
          }

          zimg(x, y).colour = (1.0 / count) * zimg(x, y).colour;
          zimg(x, y).alpha  = (1.0 / count) * zimg(x, y).alpha;
        } else {
          // grid
          int dim = 3;
          double dimn2 = 1.0 / (dim * dim);
          zimg(x, y).colour = Colour(0,0,0);
          zimg(x, y).alpha  = 0;
          for (int i = 0; i < dim; ++i) for (int j = 0; j < dim; ++j) {
            double cx = (double)width / 2.0  - (x + (2.0*i + 1)/(2.0*dim));
            double cy = (double)height / 2.0 - (y + (2.0*j + 1)/(2.0*dim));

            if (resolution == 2) {
              // jittered grid
              cx += ((rand() % 1024) / 1024.0 - 0.5) / dim;
              cy += ((rand() % 1024) / 1024.0 - 0.5) / dim;
            }

            Vector3D ray = (Z + cx * fov_scale * X + cy * fov_scale * Y).unit();
            Intersection2 display = get_colour(root, eye, ray);

            if (display) {
              zimg(x, y).colour  = zimg(x, y).colour + dimn2 * display.colour;
              zimg(x, y).alpha  += dimn2;
            }
          }
        }
      }
    }

    complete_progress("anti-aliasing");
  }*/

  /////////////////////////////////////
  // Save the image.

  Image image(width, height, 3); // the 3 is for RGB
  zimg.write(image);

  if (cmd_options().count("altfile")) {
    image.savePng(filename);
  } else {
    image.savePng("scene.png");
  }
}

