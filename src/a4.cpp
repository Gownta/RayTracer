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
  // Save the image.

  Image image(width, height, 3); // the 3 is for RGB
  zimg.write(image);

  if (cmd_options().count("altfile")) {
    image.savePng(filename);
  } else {
    image.savePng("scene.png");
  }
}

